package agent

import (
	"context"
	"errors"
	"fmt"
	"strconv"
	"strings"
	"sync"

	"github.com/golang/protobuf/ptypes/wrappers"
	"github.com/pinpoint-apm/pinpoint-c-agent/collector-agent/common"
	v1 "github.com/pinpoint-apm/pinpoint-c-agent/collector-agent/pinpoint-grpc-idl-go/proto/v1"
	"github.com/spaolacci/murmur3"

	log "github.com/sirupsen/logrus"
	"google.golang.org/grpc/metadata"
	"google.golang.org/protobuf/types/known/wrapperspb"
)

type ApiIdMap map[string]interface{}

var unique_id_count = int32(1)

type SpanSender struct {
	sequenceId          int32
	idMap               ApiIdMap
	Md                  metadata.MD
	exitCh              chan bool
	spanMessageBufferCh chan *v1.PSpanMessage
	sendStreamRespCh    chan int32
	wg                  *sync.WaitGroup
	log                 *log.Entry
}

func createSpanSender(base metadata.MD, exitCh chan bool, agent_wg *sync.WaitGroup, log *log.Entry) *SpanSender {
	sender := &SpanSender{
		Md:     base,
		exitCh: exitCh,
		idMap:  make(ApiIdMap),
		wg:     agent_wg,
		log:    log,
	}
	sender.Init()
	return sender
}

func (spanSender *SpanSender) sendSpan() {
	config := common.GetConfig()
	conn, err := common.CreateGrpcConnection(config.SpanAddress)
	if err != nil {
		spanSender.log.Warnf("connect:%s failed. %s", config.SpanAddress, err)
		return
	}
	defer conn.Close()
	client := v1.NewSpanClient(conn)

	ctx := metadata.NewOutgoingContext(context.Background(), spanSender.Md)

	stream, err := client.SendSpan(ctx)
	if err != nil {
		spanSender.log.Warnf("create stream failed. %s", err)
		return
	}
	defer stream.CloseSend()

	// for span := range spanSender.spanMessageBufferCh {
	// 	spanSender.log.Debugf("send %v", span)

	// 	if err := stream.Send(span); err != nil {
	// 		spanSender.log.Warnf("send span failed with:%s", err)
	// 		// response the stream is not available
	// 		spanSender.sendStreamRespCh <- 500
	// 		return
	// 	}
	// }
	// watch exitCh
	for {
		select {
		case span := <-spanSender.spanMessageBufferCh:
			spanSender.log.Debugf("send %v", span)

			if err := stream.Send(span); err != nil {
				spanSender.log.Warnf("send span failed with:%s", err)
				// response the stream is not available
				spanSender.sendStreamRespCh <- 500
				return
			}
		case <-spanSender.exitCh:
			spanSender.log.Warn("sendSpan failed with agent exiting")
			return
		}
	}

}

func (spanSender *SpanSender) sendThread() {
	defer spanSender.wg.Done()

	for {
		spanSender.sendSpan()
		config := common.GetConfig()
		if common.WaitChannelEvent(spanSender.exitCh, config.SpanTimeWait) == common.E_AGENT_STOPPING {
			break
		}
	}
	spanSender.log.Info("sendThread exit")
}

func (spanSender *SpanSender) Init() {
	// spanSender.sqlMeta = MetaData{MetaDataType: common.META_SQL_UID, IDMap: make(PARAMS_TYPE), Sender: spanSender}
	// spanSender.apiMeta = MetaData{MetaDataType: common.META_API, IDMap: make(PARAMS_TYPE), Sender: spanSender}
	// spanSender.stringMeta = MetaData{MetaDataType: common.META_STRING, IDMap: make(PARAMS_TYPE), Sender: spanSender}

	spanSender.spanMessageBufferCh = make(chan *v1.PSpanMessage, common.GetConfig().AgentChannelSize)
	spanSender.sendStreamRespCh = make(chan int32, 1)
	spanSender.log.Debug("SpanSender::Init span spanSender thread start")
	for i := int32(0); i < common.GetConfig().SpanStreamParallelismSize; i++ {
		spanSender.wg.Add(1)
		go spanSender.sendThread()
	}
	spanSender.log.Debug("SpanSender::Init done")
}

func (spanSender *SpanSender) cleanAllMetaData() {
	spanSender.log.Info("Clean all metaData")
	spanSender.idMap = make(ApiIdMap)
}

func (spanSender *SpanSender) makePinpointSpanEv(genSpan *v1.PSpan, spanEv *TSpanEvent, depth int32) error {
	if pbSpanEv, err := spanSender.createPinpointSpanEv(spanEv); err == nil {
		pbSpanEv.Sequence = spanSender.sequenceId
		spanSender.sequenceId += 1
		pbSpanEv.Depth = depth
		genSpan.SpanEvent = append(genSpan.SpanEvent, pbSpanEv)
		for _, call := range spanEv.Calls {
			spanSender.makePinpointSpanEv(genSpan, call, depth+1)
		}
		return nil
	} else {
		return err
	}
}

func (spanSender *SpanSender) getMetaApiId(name string, metaType int32) int32 {
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

func (spanSender *SpanSender) createPinpointSpanEv(spanEv *TSpanEvent) (*v1.PSpanEvent, error) {
	pbSpanEv := &v1.PSpanEvent{}

	pbSpanEv.ApiId = spanSender.getMetaApiId(spanEv.Name, common.META_Default_api)

	if len(spanEv.ExceptionInfo) > 0 {
		id := spanSender.getMetaApiId("___EXP___", common.META_String_api)
		pbSpanEv.ExceptionInfo = &v1.PIntStringValue{}
		pbSpanEv.ExceptionInfo.IntValue = id
		stringValue := wrapperspb.StringValue{Value: spanEv.ExceptionInfo}
		pbSpanEv.ExceptionInfo.StringValue = &stringValue
	}

	nextEv := v1.PMessageEvent{
		DestinationId: spanEv.DestinationId,
		NextSpanId:    spanEv.NextSpanId,
		EndPoint:      spanEv.EndPoint,
	}

	pbSpanEv.NextEvent = &v1.PNextEvent{
		Field: &v1.PNextEvent_MessageEvent{
			MessageEvent: &nextEv},
	}

	pbSpanEv.StartElapsed = spanEv.GetStartElapsed()

	pbSpanEv.EndElapsed = spanEv.GetEndElapsed()

	pbSpanEv.ServiceType = spanEv.ServiceType
	for _, ann := range spanEv.Clues {
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

	if len(spanEv.SqlMeta) > 0 {
		id := spanSender.getSqlUidMetaApiId(spanEv.SqlMeta)
		sqlByteSv := &v1.PBytesStringStringValue{
			BytesValue: id,
			StringValue1: &wrappers.StringValue{
				Value: spanEv.SqlMeta,
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

	return pbSpanEv, nil
}

func (spanSender *SpanSender) makePinpointSpan(span *TSpan) (*v1.PSpan, error) {
	spanSender.sequenceId = 0
	pbSpan := &v1.PSpan{}
	pbSpan.Version = 1
	pbSpan.ApiId = spanSender.getMetaApiId(span.GetAppid(), common.META_Web_request_api)

	pbSpan.ServiceType = span.ServerType

	pbSpan.ApplicationServiceType = span.GetAppServerType()

	pbSpan.ParentSpanId = span.ParentSpanId

	pbSpan.TransactionId = common.TypeV1_String_TransactionId(span.TransactionId)

	pbSpan.SpanId = span.SpanId

	pbSpan.StartTime = span.GetStartTime()

	pbSpan.Elapsed = span.GetElapsedTime()

	parentInfo := v1.PParentInfo{
		ParentApplicationName: span.ParentApplicationName,
		ParentApplicationType: span.ParentAppServerType,
		AcceptorHost:          span.AcceptorHost,
	}

	acceptEv := v1.PAcceptEvent{Rpc: span.Uri, EndPoint: span.EndPoint, RemoteAddr: span.RemoteAddr, ParentInfo: &parentInfo}

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

	for _, annotation := range span.Clues {
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
		pvalue := v1.PAnnotationValue_LongIntIntByteByteStringValue{
			LongIntIntByteByteStringValue: &v1.PLongIntIntByteByteStringValue{},
		}
		pvalue.LongIntIntByteByteStringValue.IntValue1 = 2
		ngFormat := common.ParseStringField(span.NginxHeader)
		if value, OK := ngFormat["D"]; OK {
			if value, err := common.ParseDotFormatToTime(value); err == nil {
				pvalue.LongIntIntByteByteStringValue.IntValue2 = int32(value)
			}
		}
		if value, OK := ngFormat["t"]; OK {
			if value, err := common.ParseDotFormatToTime(value); err == nil {
				pvalue.LongIntIntByteByteStringValue.LongValue = value
			}
		}

		annotation := v1.PAnnotation{
			Key: 300,
			Value: &v1.PAnnotationValue{
				Field: &pvalue,
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

func (spanSender *SpanSender) makeSpan(span *TSpan) (*v1.PSpan, error) {
	if pSpan, err := spanSender.makePinpointSpan(span); err == nil {
		for _, call := range span.Calls {
			spanSender.makePinpointSpanEv(pSpan, call, 1)
		}
		return pSpan, nil
	} else {
		return nil, err
	}
}

func (spanSender *SpanSender) Interceptor(span *TSpan) bool {
	spanSender.log.Debug("span spanSender interceptor")
	if pbSpan, err := spanSender.makeSpan(span); err == nil {
		// recv the channel status
		select {
		case spanSender.spanMessageBufferCh <- &v1.PSpanMessage{
			Field: &v1.PSpanMessage_Span{
				Span: pbSpan,
			},
		}:
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

func (spanSender *SpanSender) SenderGrpcMetaData(name string, metaType int32) error {
	config := common.GetConfig()
	conn, err := common.CreateGrpcConnection(config.AgentAddress)
	if err != nil {
		spanSender.log.Warnf("connect:%s failed. %s", config.AgentAddress, err)
		return errors.New("SenderGrpcMetaData: connect failed")
	}

	defer conn.Close()
	client := v1.NewMetadataClient(conn)

	ctx, cancel := common.BuildPinpointCtx(config.MetaDataTimeWait, spanSender.Md)
	defer cancel()

	switch metaType {
	case common.META_Default_api:
		{
			id := spanSender.idMap[name].(int32)
			apiMeta := v1.PApiMetaData{ApiId: id, ApiInfo: name, Type: common.API_DEFAULT}

			if _, err = client.RequestApiMetaData(ctx, &apiMeta); err != nil {
				spanSender.log.Warnf("agentOnline api meta failed %s", err)
				return errors.New("SenderGrpcMetaData: PApiMetaData failed")
			}
		}

	case common.META_Web_request_api:
		{
			id := spanSender.idMap[name].(int32)
			apiMeta := v1.PApiMetaData{ApiId: id, ApiInfo: name, Type: common.API_WEB_REQUEST}

			if _, err = client.RequestApiMetaData(ctx, &apiMeta); err != nil {
				spanSender.log.Warnf("agentOnline api meta failed %s", err)
				return errors.New("SenderGrpcMetaData: PApiMetaData failed")
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
				return errors.New("SenderGrpcMetaData: RequestStringMetaData failed")
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
				return errors.New("SenderGrpcMetaData: RequestSqlUidMetaData failed")
			}
		}
	default:
		spanSender.log.Warnf("SenderGrpcMetaData: No such Type:%d", metaType)
	}

	spanSender.log.Debugf("send metaData %s", name)
	return nil
}
