package agent

import (
	"context"
	"fmt"
	"strconv"
	"strings"
	"sync"

	"github.com/golang/protobuf/ptypes/wrappers"
	"github.com/pinpoint-apm/pinpoint-c-agent/collector-agent/common"
	v1 "github.com/pinpoint-apm/pinpoint-c-agent/collector-agent/pinpoint-grpc-idl-go/proto/v1"
	"github.com/sirupsen/logrus"
	"github.com/spaolacci/murmur3"

	"google.golang.org/grpc/metadata"
	"google.golang.org/protobuf/types/known/wrapperspb"
)

type ApiIdMap map[string]interface{}

var unique_id_count = int32(1)

type SpanSender struct {
	sequenceId int32
	idMap      ApiIdMap
	Md         metadata.MD
	// exitCh              chan bool
	ctx                 context.Context
	spanMessageBufferCh chan *v1.PSpanMessage
	sendStreamRespCh    chan int32
	wg                  *sync.WaitGroup
	log                 *logrus.Entry
	config              *common.Config
}

func createSpanSender(base metadata.MD, ctx context.Context, agent_wg *sync.WaitGroup, config *common.Config, entry *logrus.Entry) *SpanSender {
	sender := &SpanSender{
		Md:                  base,
		ctx:                 ctx,
		idMap:               make(ApiIdMap),
		wg:                  agent_wg,
		log:                 entry,
		config:              config,
		spanMessageBufferCh: make(chan *v1.PSpanMessage, config.AgentChannelSize),
		sendStreamRespCh:    make(chan int32, 1),
	}
	sender.StartServe()
	return sender
}

func (s *SpanSender) Stop() {}

func (s *SpanSender) sendSpan() {
	conn, err := s.config.CreateGrpcConnection(s.ctx, s.config.User.SpanAddress)
	if err != nil {
		s.log.Warnf("connect:%s failed. %s", s.config.User.SpanAddress, err)
		return
	}
	defer conn.Close()
	client := v1.NewSpanClient(conn)

	ctx := metadata.NewOutgoingContext(s.ctx, s.Md)
	stream, err := client.SendSpan(ctx)
	if err != nil {
		s.log.Warnf("create stream failed. %s", err)
		return
	}
	defer stream.CloseSend()
	for {
		select {
		case span := <-s.spanMessageBufferCh:
			s.log.Debugf("send %v", span)

			if err := stream.Send(span); err != nil {
				s.log.Warnf("send span failed with:%s", err)
				// response the stream is not available
				s.sendStreamRespCh <- 500
				return
			}
		case <-s.ctx.Done():
			s.log.Warn("sendSpan failed with agent exiting")
			return
		}
	}

}

func (spanSender *SpanSender) sendTask() {
	defer spanSender.wg.Done()

	for {
		spanSender.sendSpan()
		if common.WaitEventsWithTime(spanSender.ctx, spanSender.config.SpanTimeWait) == common.E_AGENT_STOPPING {
			break
		}
	}
	spanSender.log.Info("sendThread exit")
}

func (spanSender *SpanSender) StartServe() {
	spanSender.log.Debug("SpanSender::StartServe span spanSender thread start")
	for i := int32(0); i < spanSender.config.SpanStreamParallelismSize; i++ {
		spanSender.wg.Add(1)
		go spanSender.sendTask()
	}
	spanSender.log.Debug("SpanSender::StartServe done")
}

func (spanSender *SpanSender) cleanAllMetaData() {
	spanSender.log.Info("Clean all metaData")
	spanSender.idMap = make(ApiIdMap)
}

func (spanSender *SpanSender) makeSpanEvent(spanEv *TSpanEvent) *v1.PSpanEvent {
	return spanSender.createPinpointSpanEv(spanEv)
}

func (spanSender *SpanSender) getMetaApiId(name string, metaType common.Meta_Type) int32 {
	id, ok := spanSender.idMap[name]
	if ok {
		return id.(int32)
	} else {
		unique_id_count += 1
		spanSender.idMap[name] = unique_id_count
		spanSender.SenderGrpcMetaData(name, metaType)
		return unique_id_count
	}
}

func (spanSender *SpanSender) getSqlUidMetaApiId(name string) []byte {
	id, ok := spanSender.idMap[name]
	if ok {
		return id.([]byte)
	} else {
		h1, h2 := murmur3.Sum128([]byte(name))
		// use %x to format hash
		id := []byte(fmt.Sprintf("%x%x", h1, h2))
		spanSender.idMap[name] = id
		spanSender.SenderGrpcMetaData(name, common.META_Sql_uid_api)
		return id
	}
}

func (spanSender *SpanSender) createPinpointSpanEv(spanEv *TSpanEvent) *v1.PSpanEvent {
	pbSpanEv := &v1.PSpanEvent{
		StartElapsed: spanEv.GetStartElapsed(),
		EndElapsed:   spanEv.GetEndElapsed(),
		ServiceType:  spanEv.ServiceType,
		Depth:        spanEv.Depth,
		Sequence:     spanEv.Sequence,
		AsyncEvent:   spanEv.AsyId,
	}

	// TODO check serverType is 100 ,100 should be META_INVOCATION_API
	// from https://github.com/pinpoint-apm/pinpoint/blob/91af11b99d94c7ce5076a8bf468bf02480eb129d/agent-module/profiler/src/main/java/com/navercorp/pinpoint/profiler/context/method/AsyncMethodDescriptor.java#L26
	const API_INVOCATION_SERVICE_TYPE = 100
	if spanEv.ServiceType == API_INVOCATION_SERVICE_TYPE {
		pbSpanEv.ApiId = spanSender.getMetaApiId(spanEv.Name, common.META_INVOCATION_API)
	} else {
		pbSpanEv.ApiId = spanSender.getMetaApiId(spanEv.Name, common.META_Default_api)
	}

	if len(spanEv.ExceptionInfo) > 0 {
		id := spanSender.getMetaApiId("___EXP___", common.META_String_api)
		pbSpanEv.ExceptionInfo = &v1.PIntStringValue{}
		pbSpanEv.ExceptionInfo.IntValue = id
		stringValue := wrapperspb.StringValue{Value: spanEv.ExceptionInfo}
		pbSpanEv.ExceptionInfo.StringValue = &stringValue
	}

	nextEv := &v1.PMessageEvent{
		DestinationId: spanEv.DestinationId,
		NextSpanId:    spanEv.NextSpanId,
		EndPoint:      spanEv.EndPoint,
	}

	pbSpanEv.NextEvent = &v1.PNextEvent{
		Field: &v1.PNextEvent_MessageEvent{
			MessageEvent: nextEv,
		},
	}

	for _, ann := range spanEv.Annotations {
		iColon := strings.Index(ann, ":")
		if value, err := strconv.ParseInt(ann[0:iColon], 10, 32); err == nil {
			stringValue := v1.PAnnotationValue_StringValue{StringValue: ann[iColon+1:]}

			v := v1.PAnnotationValue{
				Field: &stringValue,
			}
			ann := v1.PAnnotation{
				Key:   int32(value),
				Value: &v,
			}
			pbSpanEv.Annotation = append(pbSpanEv.Annotation, &ann)
		}
	}

	if spanEv.SqlMeta != nil {
		id := spanSender.getSqlUidMetaApiId(*spanEv.SqlMeta)
		sqlByteSv := &v1.PBytesStringStringValue{
			BytesValue: id,
			StringValue1: &wrappers.StringValue{
				Value: *spanEv.SqlMeta,
			},
		}
		pbSpanEv.Annotation = append(pbSpanEv.Annotation, &v1.PAnnotation{
			Key: 25,
			Value: &v1.PAnnotationValue{
				Field: &v1.PAnnotationValue_BytesStringStringValue{
					BytesStringStringValue: sqlByteSv,
				},
			},
		})
	}

	return pbSpanEv
}

func (spanSender *SpanSender) makeSpanChunk(span *TSpan) (*v1.PSpanChunk, error) {
	chunk := &v1.PSpanChunk{Version: 1,
		TransactionId:          common.TypeV1_String_TransactionId(span.TransactionId),
		SpanId:                 span.SpanId,
		KeyTime:                span.GetStartTime(),
		ApplicationServiceType: span.AppServerType,
		EndPoint:               span.EndPoint,
		LocalAsyncId: &v1.PLocalAsyncId{
			AsyncId:  span.LocalAsyncId.AsyncId,
			Sequence: span.LocalAsyncId.Sequence,
		},
	}
	return chunk, nil
}

func (spanSender *SpanSender) makePinpointSpan(span *TSpan) (*v1.PSpan, error) {
	spanSender.sequenceId = 0
	pbSpan := &v1.PSpan{
		ParentSpanId:           -1,
		Version:                1,
		ServiceType:            span.ServerType,
		ApplicationServiceType: span.GetAppServerType(),
		SpanId:                 span.SpanId,
		StartTime:              span.GetStartTime(),
		Elapsed:                span.GetElapsedTime(),
		TransactionId:          common.TypeV1_String_TransactionId(span.TransactionId),
		ApiId:                  spanSender.getMetaApiId(span.SpanName, common.META_Web_request_api),
	}

	if span.ParentSpanId > 0 {
		pbSpan.ParentSpanId = span.ParentSpanId
	}

	parentInfo := v1.PParentInfo{
		ParentApplicationName: span.ParentApplicationName,
		ParentApplicationType: span.ParentAppServerType,
		AcceptorHost:          span.AcceptorHost,
	}

	acceptEv := v1.PAcceptEvent{
		Rpc:        span.Uri,
		EndPoint:   span.EndPoint,
		RemoteAddr: span.RemoteAddr,
		ParentInfo: &parentInfo,
	}

	pbSpan.AcceptEvent = &acceptEv
	// changes: ERRs's priority bigger EXP, so ERR will replace EXP
	if len(span.ExceptionInfo) > 0 {
		id := spanSender.getMetaApiId("___EXP___", common.META_String_api)
		stringValue := wrapperspb.StringValue{Value: span.ExceptionInfo}
		pbSpan.ExceptionInfo = &v1.PIntStringValue{IntValue: id,
			StringValue: &stringValue}
	}

	if span.ErrorInfo != nil {
		id := spanSender.getMetaApiId("___ERR___", common.META_String_api)
		pbSpan.Err = 1 // mark as an error
		pbSpan.ExceptionInfo = &v1.PIntStringValue{
			IntValue: id,
			StringValue: &wrapperspb.StringValue{
				Value: span.ErrorInfo.Msg,
			},
		}
	}

	for _, annotation := range span.Annotations {
		iColon := strings.Index(annotation, ":")
		if iColon > 0 {
			if value, err := strconv.ParseInt(annotation[0:iColon], 10, 32); err == nil {
				stringValue := v1.PAnnotationValue_StringValue{StringValue: annotation[iColon+1:]}
				pAnn := v1.PAnnotationValue{
					Field: &stringValue,
				}
				ann := v1.PAnnotation{
					Key:   int32(value),
					Value: &pAnn,
				}
				pbSpan.Annotation = append(pbSpan.Annotation, &ann)
			}
		}
	}

	// collector data from nginx-header
	if len(span.NginxHeader) > 0 {
		ann := v1.PAnnotationValue_LongIntIntByteByteStringValue{
			LongIntIntByteByteStringValue: &v1.PLongIntIntByteByteStringValue{},
		}
		ann.LongIntIntByteByteStringValue.IntValue1 = 2
		ngFormat := common.ParseStringField(span.NginxHeader)
		if value, OK := ngFormat["D"]; OK {
			if value, err := common.ParseDotFormatToTime(value); err == nil {
				ann.LongIntIntByteByteStringValue.IntValue2 = int32(value)
			}
		}
		if value, OK := ngFormat["t"]; OK {
			if value, err := common.ParseDotFormatToTime(value); err == nil {
				ann.LongIntIntByteByteStringValue.LongValue = value
			}
		}

		annotation := v1.PAnnotation{
			Key: 300,
			Value: &v1.PAnnotationValue{
				Field: &ann,
			},
		}
		pbSpan.Annotation = append(pbSpan.Annotation, &annotation)
	}
	// collect data from apache-header
	if len(span.ApacheHeader) > 0 {
		pvalue := v1.PAnnotationValue_LongIntIntByteByteStringValue{
			LongIntIntByteByteStringValue: &v1.PLongIntIntByteByteStringValue{},
		}
		pvalue.LongIntIntByteByteStringValue.IntValue1 = 3
		npAr := common.ParseStringField(span.ApacheHeader)
		if value, OK := npAr["i"]; OK {
			if value, err := strconv.ParseInt(value, 10, 32); err == nil {
				pvalue.LongIntIntByteByteStringValue.ByteValue1 = int32(value)
			}
		}
		if value, OK := npAr["b"]; OK {
			if value, err := strconv.ParseInt(value, 10, 32); err == nil {
				pvalue.LongIntIntByteByteStringValue.ByteValue2 = int32(value)
			}
		}
		if value, OK := npAr["D"]; OK {
			if value, err := strconv.ParseInt(value, 10, 32); err == nil {
				pvalue.LongIntIntByteByteStringValue.IntValue2 = int32(value)
			}
		}
		if value, OK := npAr["t"]; OK {
			if value, err := strconv.ParseInt(value, 10, 64); err == nil {
				pvalue.LongIntIntByteByteStringValue.LongValue = value / 1000
			}
		}

		ann := v1.PAnnotation{
			Key: 300,
			Value: &v1.PAnnotationValue{
				Field: &pvalue,
			},
		}

		pbSpan.Annotation = append(pbSpan.Annotation, &ann)
	}

	return pbSpan, nil
}

func (spanSender *SpanSender) makeSpanOrSpanChunk(span *TSpan) (*v1.PSpan, *v1.PSpanChunk, error) {

	var spanEv []*v1.PSpanEvent
	for _, call := range span.Follows {
		spanEv = append(spanEv, spanSender.makeSpanEvent(call))
	}

	if span.LocalAsyncId == nil {
		pSpan, _ := spanSender.makePinpointSpan(span)
		pSpan.SpanEvent = spanEv
		return pSpan, nil, nil
	} else {
		chunk, _ := spanSender.makeSpanChunk(span)
		chunk.SpanEvent = spanEv
		return nil, chunk, nil
	}
}

func (spanSender *SpanSender) Interceptor(span *TSpan) bool {
	spanSender.log.Debug("span spanSender interceptor")
	if pbSpan, pbChunk, err := spanSender.makeSpanOrSpanChunk(span); err == nil {
		spanMessage := &v1.PSpanMessage{}
		if pbSpan != nil {
			spanMessage.Field = &v1.PSpanMessage_Span{
				Span: pbSpan,
			}
		} else {
			spanMessage.Field = &v1.PSpanMessage_SpanChunk{
				SpanChunk: pbChunk,
			}
		}

		select {
		case spanSender.spanMessageBufferCh <- spanMessage:
		case statusCode := <-spanSender.sendStreamRespCh:
			spanSender.log.Warnf("span send stream is offline statusCode:%d, clear all string/sql/api meta data", statusCode)
			spanSender.cleanAllMetaData()
		default:
			spanSender.log.Warn("current span dropped, due to spanStream slow or disconnected and spanMessageBufferCh is full")
		}
	} else {
		spanSender.log.Warnf("SpanSender::Interceptor return err:%s", err)
	}
	return true
}

func (spanSender *SpanSender) SenderGrpcMetaData(name string, metaType common.Meta_Type) {
	conn, err := spanSender.config.CreateGrpcConnection(spanSender.ctx, spanSender.config.User.AgentAddress)
	if err != nil {
		spanSender.log.Warnf("connect:%s failed. %s", spanSender.config.User.AgentAddress, err)
		return
	}

	defer conn.Close()
	client := v1.NewMetadataClient(conn)

	ctx, cancel := context.WithTimeout(spanSender.ctx, spanSender.config.MetaDataTimeWait)

	ctx = metadata.NewOutgoingContext(ctx, spanSender.Md)

	defer cancel()

	switch metaType {
	case common.META_Default_api:
		{
			id := spanSender.idMap[name].(int32)
			apiMeta := v1.PApiMetaData{ApiId: id, ApiInfo: name, Type: int32(common.API_DEFAULT)}

			if _, err = client.RequestApiMetaData(ctx, &apiMeta); err != nil {
				spanSender.log.Warnf("agentOnline api meta failed %s", err)
				delete(spanSender.idMap, name)
			}
		}

	case common.META_Web_request_api:
		{
			id := spanSender.idMap[name].(int32)
			apiMeta := v1.PApiMetaData{ApiId: id, ApiInfo: name, Type: int32(common.API_WEB_REQUEST)}

			if _, err = client.RequestApiMetaData(ctx, &apiMeta); err != nil {
				spanSender.log.Warnf("agentOnline api meta failed %s", err)
				delete(spanSender.idMap, name)
			}
		}
	case common.META_String_api:
		{
			id := spanSender.idMap[name].(int32)
			metaMeta := v1.PStringMetaData{
				StringId:    id,
				StringValue: name,
			}

			if _, err = client.RequestStringMetaData(ctx, &metaMeta); err != nil {
				spanSender.log.Warnf("agentOnline api meta failed %s", err)
				delete(spanSender.idMap, name)
			}
		}

	case common.META_Sql_uid_api:
		{
			id := spanSender.idMap[name].([]byte)
			sqlUidMeta := v1.PSqlUidMetaData{
				SqlUid: id,
				Sql:    name,
			}
			if _, err = client.RequestSqlUidMetaData(ctx, &sqlUidMeta); err != nil {
				spanSender.log.Warnf("agentOnline api meta failed %s", err)
				delete(spanSender.idMap, name)
			}
		}
	case common.META_INVOCATION_API:
		{
			id := spanSender.idMap[name].(int32)
			apiMeta := v1.PApiMetaData{ApiId: id, ApiInfo: name, Type: int32(common.API_INVOCATION)}
			if _, err = client.RequestApiMetaData(ctx, &apiMeta); err != nil {
				spanSender.log.Warnf("agentOnline api meta failed %s", err)
				delete(spanSender.idMap, name)
			}
		}
	default:
		spanSender.log.Warnf("SenderGrpcMetaData: No such Type:%d", metaType)
	}

	spanSender.log.Debugf("send metaData name:%s type:%d ", name, metaType)
}
