package agent

import (
	"context"
	"errors"
	"strconv"
	"strings"
	"sync"
	"time"

	"github.com/golang/protobuf/ptypes/wrappers"
	"github.com/pinpoint-apm/pinpoint-c-agent/collector-agent/common"
	v1 "github.com/pinpoint-apm/pinpoint-c-agent/collector-agent/pinpoint-grpc-idl/proto/v1"

	log "github.com/sirupsen/logrus"
	"google.golang.org/grpc/metadata"
	"google.golang.org/protobuf/types/known/wrapperspb"
)

type SpanSender struct {
	sequenceId    int32
	apiMeta       MetaData
	stringMeta    MetaData
	sqlMeta       MetaData
	Md            metadata.MD
	ExitCh        chan bool
	spanMessageCh chan *v1.PSpanMessage
	spanRespCh    chan int32
	wg            sync.WaitGroup
}

func (spanSender *SpanSender) Stop() {
	log.Warn("Try to close spanSend goroutine")
	close(spanSender.spanMessageCh)
	close(spanSender.spanRespCh)
	spanSender.wg.Wait()
	log.Warn("Span sender goroutine exit")
}

func (spanSender *SpanSender) senderMain() {
	config := common.GetConfig()
	conn, err := common.CreateGrpcConnection(config.SpanAddress)
	if err != nil {
		log.Warnf("connect:%s failed. %s", config.SpanAddress, err)
		return
	}
	defer conn.Close()
	client := v1.NewSpanClient(conn)

	ctx := metadata.NewOutgoingContext(context.Background(), spanSender.Md)

	stream, err := client.SendSpan(ctx)
	if err != nil {
		log.Warnf("create streamfailed. %s", err)
		return
	}
	defer stream.CloseSend()

	for span := range spanSender.spanMessageCh {
		log.Debugf("send %v", span)

		if err := stream.Send(span); err != nil {
			log.Warnf("send span failed with:%s", err)
			// response the stream is not available
			spanSender.spanRespCh <- 500
			return
		}
	}
}

func (spanSender *SpanSender) sendThread() {
	defer spanSender.wg.Done()

	spanSender.wg.Add(1)
	for {
		spanSender.senderMain()
		config := common.GetConfig()
		if common.WaitChannelEvent(spanSender.ExitCh, config.SpanTimeWaitSec) == common.E_AGENT_STOPPING {
			break
		}
	}
	log.Info("sendThread exit")
}

func (spanSender *SpanSender) Init() {
	spanSender.sqlMeta = MetaData{MetaDataType: common.META_SQL, IDMap: make(PARAMS_TYPE), Sender: spanSender}
	spanSender.apiMeta = MetaData{MetaDataType: common.META_API, IDMap: make(PARAMS_TYPE), Sender: spanSender}
	spanSender.stringMeta = MetaData{MetaDataType: common.META_STRING, IDMap: make(PARAMS_TYPE), Sender: spanSender}

	spanSender.spanMessageCh = make(chan *v1.PSpanMessage, common.GetConfig().AgentChannelSize)
	spanSender.spanRespCh = make(chan int32, 1)
	log.Debug("SpanSender::Init span spanSender thread start")
	for i := int32(0); i < common.GetConfig().SpanStreamParallelismSize; i++ {
		go spanSender.sendThread()
	}
	log.Debug("SpanSender::Init done")
}

func (spanSender *SpanSender) cleanAllMetaData() {
	log.Info("Clean all metaData")
	spanSender.sqlMeta.ResetMeta()
	spanSender.apiMeta.ResetMeta()
	spanSender.stringMeta.ResetMeta()
}

func (spanSender *SpanSender) makePinpointSpanEv(genSpan *v1.PSpan, spanEv *TSpanEvent, depth int32) error {
	if pbSpanEv, err := spanSender.createPinpointSpanEv(spanEv); err == nil {
		pbSpanEv.Sequence = spanSender.sequenceId
		spanSender.sequenceId += 1
		pbSpanEv.Depth = depth
		genSpan.SpanEvent = append(genSpan.SpanEvent, pbSpanEv)
		for _, call := range spanEv.Calls {
			spanSender.makePinpointSpanEv(genSpan, &call, depth+1)
		}
		return nil
	} else {
		return err
	}
}

func (spanSender *SpanSender) createPinpointSpanEv(spanEv *TSpanEvent) (*v1.PSpanEvent, error) {
	pbSpanEv := &v1.PSpanEvent{}

	pbSpanEv.ApiId = int32(spanSender.apiMeta.GetId(spanEv.Name, PARAMS_TYPE{"type": common.API_DEFAULT}))

	if len(spanEv.ExceptionInfo) > 0 {
		id := spanSender.stringMeta.GetId("EXP", nil)
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

			pAnvalue := v1.PAnnotationValue{
				Field: &stringValue,
			}
			ann := v1.PAnnotation{
				Key:   int32(value),
				Value: &pAnvalue,
			}
			pbSpanEv.Annotation = append(pbSpanEv.Annotation, &ann)
		}
	}

	if len(spanEv.SqlMeta) > 0 {
		id := spanSender.sqlMeta.GetId(spanEv.SqlMeta, nil)
		sqlvalue := v1.PIntStringStringValue{
			IntValue: id,
			StringValue1: &wrappers.StringValue{
				Value: spanEv.SqlMeta,
			},
		}
		pbSpanEv.Annotation = append(pbSpanEv.Annotation, &v1.PAnnotation{
			Key: 20,
			Value: &v1.PAnnotationValue{
				Field: &v1.PAnnotationValue_IntStringStringValue{
					IntStringStringValue: &sqlvalue,
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
	pbSpan.ApiId = spanSender.apiMeta.GetId(span.GetAppid(), PARAMS_TYPE{"type": int32(common.API_WEB_REQUEST)})

	pbSpan.ServiceType = span.ServerType

	pbSpan.ApplicationServiceType = span.GetAppServerType()

	pbSpan.ParentSpanId = span.ParentSpanId

	tidFormat := strings.Split(span.TransactionId, "^")
	agentId, startTime, seqenceid := tidFormat[0], tidFormat[1], tidFormat[2]
	transactionId := v1.PTransactionId{AgentId: agentId}

	if value, err := strconv.ParseInt(startTime, 10, 64); err == nil {
		transactionId.AgentStartTime = value
	}

	if value, err := strconv.ParseInt(seqenceid, 10, 64); err == nil {
		transactionId.Sequence = value
	}
	pbSpan.TransactionId = &transactionId

	pbSpan.SpanId = span.SpanId

	pbSpan.StartTime = span.GetStartTime()

	pbSpan.Elapsed = span.GetElapsedTime()

	parentInfo := v1.PParentInfo{
		ParentApplicationName: span.ParentApplicationName,
		ParentApplicationType: int32(span.ParentAppServerType),
		AcceptorHost:          span.AcceptorHost,
	}

	acceptEv := v1.PAcceptEvent{Rpc: span.Uri, EndPoint: span.EndPoint, RemoteAddr: span.RemoteAddr,
		ParentInfo: &parentInfo}

	pbSpan.AcceptEvent = &acceptEv
	// changes: ERRs's priority bigger EXP, so ERR will replace EXP
	if len(span.ExceptionInfo) > 0 {
		id := spanSender.stringMeta.GetId("EXP", nil)
		stringValue := wrapperspb.StringValue{Value: span.ExceptionInfo}
		pbSpan.ExceptionInfo = &v1.PIntStringValue{IntValue: id,
			StringValue: &stringValue}
	}

	if span.ErrorInfo != nil {
		id := spanSender.stringMeta.GetId("ERR", nil)
		pbSpan.Err = 1 // mark as an error
		pbSpan.ExceptionInfo = &v1.PIntStringValue{
			IntValue: id,
			StringValue: &wrapperspb.StringValue{
				Value: span.ErrorInfo.Msg}}

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
	if pspan, err := spanSender.makePinpointSpan(span); err == nil {
		for _, call := range span.Calls {
			spanSender.makePinpointSpanEv(pspan, &call, 1)
		}
		return pspan, nil
	} else {
		return nil, err
	}
}

func (spanSender *SpanSender) Interceptor(span *TSpan) bool {
	log.Debug("span spanSender interceptor")
	if pbSpan, err := spanSender.makeSpan(span); err == nil {
		// send channel
		spanSender.spanMessageCh <- &v1.PSpanMessage{
			Field: &v1.PSpanMessage_Span{
				Span: pbSpan,
			},
		}
		// recv the channel status
		select {
		case statusCode := <-spanSender.spanRespCh:
			log.Warnf("span send stream is offline statusCode:%d, clear all string/sql/api meta data", statusCode)
			spanSender.cleanAllMetaData()
		case <-time.After(0 * time.Second):
			// do nothing, just go on
		}
	} else {
		log.Warnf("SpanSender::Interceptor return err:%s", err)
	}
	return true
}

func (spanSender *SpanSender) SenderGrpcMetaData(name string, id int32, Type int32, params PARAMS_TYPE) error {
	config := common.GetConfig()
	conn, err := common.CreateGrpcConnection(config.AgentAddress)
	if err != nil {
		log.Warnf("connect:%s failed. %s", config.AgentAddress, err)
		return errors.New("SenderGrpcMetaData: connect failed")
	}

	defer conn.Close()
	client := v1.NewMetadataClient(conn)

	ctx, cancel := common.BuildPinpointCtx(config.MetaDataTimeWaitSec, spanSender.Md)
	defer cancel()

	switch Type {
	case common.META_API:
		{
			//line := int32(0)
			_type := int32(common.API_DEFAULT) //
			if params != nil {
				if value, OK := params["type"]; OK {
					_type = value
				}

			}
			// disable line
			apiMeta := v1.PApiMetaData{ApiId: id, ApiInfo: name, Type: _type}

			if _, err = client.RequestApiMetaData(ctx, &apiMeta); err != nil {
				log.Warnf("agentOnline api meta failed %s", err)
				return errors.New("SenderGrpcMetaData: RequestApiMetaData failed")
			}
		}
	case common.META_STRING:
		{
			metaMeta := v1.PStringMetaData{
				StringId:    id,
				StringValue: name,
			}

			if _, err = client.RequestStringMetaData(ctx, &metaMeta); err != nil {
				log.Warnf("agentOnline api meta failed %s", err)
				return errors.New("SenderGrpcMetaData: RequestStringMetaData failed")
			}
		}
	case common.META_SQL:
		{
			sqlMeta := v1.PSqlMetaData{
				SqlId: id,
				Sql:   name,
			}

			if _, err = client.RequestSqlMetaData(ctx, &sqlMeta); err != nil {
				log.Warnf("agentOnline api meta failed %s", err)
				return errors.New("SenderGrpcMetaData: RequestSqlMetaData failed")
			}
		}
	default:
		log.Warnf("SenderGrpcMetaData: No such Type:%d", Type)
	}

	log.Debugf("send metaData type:%d,Id:%d,value:%s para:%v", Type, id, name, params)
	return nil
}
