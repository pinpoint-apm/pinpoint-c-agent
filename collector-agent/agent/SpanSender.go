package agent

import (
	"CollectorAgent/common"
	v1 "CollectorAgent/protocol"
	"context"
	"errors"
	"strconv"
	"strings"
	"sync"
	"time"

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
		log.Debugf("send Main send:%v", span)
		//if rand.Intn(100000) != 1 {
		//	continue
		//}

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

func (spanSender *SpanSender) makePinpointSpanEv(genSpan *v1.PSpan, Ispan interface{}, depth int32) error {
	if span, OK := Ispan.(map[string]interface{}); OK {
		if spanEv, err := spanSender.createPinpointSpanEv(span); err == nil {
			spanEv.Sequence = spanSender.sequenceId
			spanSender.sequenceId += 1
			spanEv.Depth = depth
			genSpan.SpanEvent = append(genSpan.SpanEvent, spanEv)
			if calls, OK := span["calls"].([]interface{}); OK {
				for _, call := range calls {
					spanSender.makePinpointSpanEv(genSpan, call, depth+1)
				}
			}
			return nil
		} else {
			return err
		}
	} else {
		panic(Ispan)
	}
}

func (spanSender *SpanSender) createPinpointSpanEv(span map[string]interface{}) (*v1.PSpanEvent, error) {
	pspanEv := v1.PSpanEvent{}
	if name, OK := span["name"].(string); OK {
		pspanEv.ApiId = int32(spanSender.apiMeta.GetId(name, PARAMS_TYPE{"type": common.API_DEFAULT}))
	} else {
		return nil, errors.New("no name")
	}

	if value, OK := span["EXP"].(string); OK {
		id := spanSender.stringMeta.GetId("EXP", nil)
		pspanEv.ExceptionInfo = &v1.PIntStringValue{}
		pspanEv.ExceptionInfo.IntValue = id
		stringValue := wrapperspb.StringValue{Value: value}
		pspanEv.ExceptionInfo.StringValue = &stringValue
	}

	nextEv := v1.PMessageEvent{}

	if value, OK := span["dst"].(string); OK {
		nextEv.DestinationId = value
	}

	if value, OK := span["nsid"].(string); OK {
		if value, err := strconv.ParseInt(value, 10, 64); err == nil {
			nextEv.NextSpanId = value
		}
	}

	if value, OK := span["server"].(string); OK {
		nextEv.EndPoint = value
	}
	pspanEv.NextEvent = &v1.PNextEvent{
		Field: &v1.PNextEvent_MessageEvent{
			MessageEvent: &nextEv},
	}

	if value, OK := span["S"].(float64); OK {
		//if value, err := strconv.ParseInt(value, 10, 32); err == nil {
		//}
		pspanEv.StartElapsed = int32(value)
	}

	if value, OK := span["E"].(float64); OK {
		pspanEv.EndElapsed = int32(value)
	}

	if value, OK := span["stp"].(string); OK {
		if itype, err := strconv.ParseInt(value, 10, 32); err == nil {
			pspanEv.ServiceType = int32(itype)
		}
	} else {
		pspanEv.ServiceType = 1501
	}

	//pspanEv.AsyncEvent = 1

	if value, OK := span["clues"].([]interface{}); OK {

		for _, ann := range value {
			if annStr, OK := ann.(string); OK {
				iColon := strings.Index(annStr, ":")
				if value, err := strconv.ParseInt(annStr[0:iColon], 10, 32); err == nil {
					stringValue := v1.PAnnotationValue_StringValue{StringValue: annStr[iColon+1:]}

					pAnvalue := v1.PAnnotationValue{
						Field: &stringValue,
					}
					ann := v1.PAnnotation{
						Key:   int32(value),
						Value: &pAnvalue,
					}
					pspanEv.Annotation = append(pspanEv.Annotation, &ann)
				}
			}
		}
	}

	if value, OK := span["SQL"].(string); OK {
		id := spanSender.sqlMeta.GetId(value, nil)
		sqlvalue := v1.PIntStringStringValue{
			IntValue: id,
		}
		pspanEv.Annotation = append(pspanEv.Annotation, &v1.PAnnotation{
			Key: 20,
			Value: &v1.PAnnotationValue{
				Field: &v1.PAnnotationValue_IntStringStringValue{
					&sqlvalue,
				},
			},
		})
	}

	return &pspanEv, nil
}

func (spanSender *SpanSender) makePinpointSpan(span map[string]interface{}) (*v1.PSpan, error) {
	spanSender.sequenceId = 0
	pspan := v1.PSpan{}
	pspan.Version = 1
	if name, OK := span["name"].(string); OK {
		pspan.ApiId = spanSender.apiMeta.GetId(name, PARAMS_TYPE{"type": int32(common.API_WEB_REQUEST)})
	} else {
		return nil, errors.New("no name")
	}

	if value, OK := span["stp"].(string); OK {
		if itype, err := strconv.ParseInt(value, 10, 32); err == nil {
			pspan.ServiceType = int32(itype)
		}
	} else {
		pspan.ServiceType = 1500
	}

	if value, OK := span["FT"].(float64); OK {
		pspan.ApplicationServiceType = int32(value)
	} else {
		pspan.ServiceType = 1500
	}

	if _type, OK := span["psid"].(string); OK {
		if itype, err := strconv.ParseInt(_type, 10, 64); err == nil {
			pspan.ParentSpanId = itype
		}
	} else {
		pspan.ParentSpanId = -1
	}

	if value, OK := span["tid"].(string); OK {
		strholder := strings.Split(value, "^")
		agentId, startTime, seqenceid := strholder[0], strholder[1], strholder[2]
		transactionId := v1.PTransactionId{AgentId: agentId}

		if value, err := strconv.ParseInt(startTime, 10, 64); err == nil {
			transactionId.AgentStartTime = value
		}

		if value, err := strconv.ParseInt(seqenceid, 10, 64); err == nil {
			transactionId.Sequence = value
		}
		pspan.TransactionId = &transactionId
	} else {
		return nil, errors.New("no tid")
	}

	if value, OK := span["sid"].(string); OK {
		if value, err := strconv.ParseInt(value, 10, 64); err == nil {
			pspan.SpanId = value
		}
	} else {
		return nil, errors.New("no sid")
	}

	if value, OK := span["S"].(float64); OK {
		pspan.StartTime = int64(value)
	} else {
		panic(span["S"])
	}

	if value, OK := span["E"].(float64); OK {
		pspan.Elapsed = int32(value)
	}
	acceptEv := v1.PAcceptEvent{}
	if value, OK := span["uri"].(string); OK {
		acceptEv.Rpc = value
	}

	if value, OK := span["server"].(string); OK {
		acceptEv.EndPoint = value
	}

	if value, OK := span["client"].(string); OK {
		acceptEv.RemoteAddr = value
	}

	parentInfo := v1.PParentInfo{}

	if value, OK := span["pname"].(string); OK {
		parentInfo.ParentApplicationName = value
	}

	if value, OK := span["ptype"].(string); OK {
		if value, err := strconv.ParseInt(value, 10, 32); err == nil {
			parentInfo.ParentApplicationType = int32(value)
		}
	}

	if value, OK := span["Ah"].(string); OK {
		parentInfo.AcceptorHost = value
	}
	acceptEv.ParentInfo = &parentInfo

	pspan.AcceptEvent = &acceptEv

	if value, OK := span["ERR"]; OK {
		id := spanSender.stringMeta.GetId("ERR", nil)
		pspan.ExceptionInfo = &v1.PIntStringValue{}
		pspan.ExceptionInfo.IntValue = id
		pspan.Err = 1 // mark as an error
		if value, OK := value.(map[string]interface{}); OK {
			if value, OK := value["msg"].(string); OK {
				stringValue := wrapperspb.StringValue{Value: value}
				pspan.ExceptionInfo.StringValue = &stringValue
			}
		}
	}

	if value, OK := span["EXP"].(string); OK {
		id := spanSender.stringMeta.GetId("EXP", nil)
		pspan.ExceptionInfo.IntValue = id
		stringValue := wrapperspb.StringValue{Value: value}
		pspan.ExceptionInfo.StringValue = &stringValue
	}

	if value, OK := span["clues"].([]interface{}); OK {
		for _, ann := range value {
			if annStr, OK := ann.(string); OK {
				iColon := strings.Index(annStr, ":")
				if iColon > 0 {
					if value, err := strconv.ParseInt(annStr[0:iColon], 10, 32); err == nil {
						stringValue := v1.PAnnotationValue_StringValue{StringValue: annStr[iColon+1:]}
						pAnn := v1.PAnnotationValue{
							Field: &stringValue,
						}
						ann := v1.PAnnotation{
							Key:   int32(value),
							Value: &pAnn,
						}
						pspan.Annotation = append(pspan.Annotation, &ann)
					}
				}

			}
		}
	}

	// collector data from nginx-header
	if value, OK := span["NP"].(string); OK {
		pvalue := v1.PAnnotationValue_LongIntIntByteByteStringValue{
			LongIntIntByteByteStringValue: &v1.PLongIntIntByteByteStringValue{},
		}
		pvalue.LongIntIntByteByteStringValue.IntValue1 = 2
		npAr := common.ParseStringField(value)
		if value, OK := npAr["D"]; OK {
			if value, err := common.ParseDotFormatToTime(value); err == nil {
				pvalue.LongIntIntByteByteStringValue.IntValue2 = int32(value)
			}
		}
		if value, OK := npAr["t"]; OK {
			if value, err := common.ParseDotFormatToTime(value); err == nil {
				pvalue.LongIntIntByteByteStringValue.LongValue = value
			}
		}

		ann := v1.PAnnotation{
			Key: 300,
			Value: &v1.PAnnotationValue{
				Field: &pvalue,
			},
		}
		pspan.Annotation = append(pspan.Annotation, &ann)
	}
	// collect data from apache-header
	if value, OK := span["AP"].(string); OK {
		pvalue := v1.PAnnotationValue_LongIntIntByteByteStringValue{
			LongIntIntByteByteStringValue: &v1.PLongIntIntByteByteStringValue{},
		}
		pvalue.LongIntIntByteByteStringValue.IntValue1 = 3
		npAr := common.ParseStringField(value)
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
	}

	return &pspan, nil
}

func (spanSender *SpanSender) makeSpan(span map[string]interface{}) (*v1.PSpan, error) {
	if pspan, err := spanSender.makePinpointSpan(span); err == nil {
		if calls, OK := span["calls"].([]interface{}); OK {
			for _, call := range calls {
				spanSender.makePinpointSpanEv(pspan, call, 1)
			}
		}
		return pspan, nil
	} else {
		return nil, err
	}
}

func (spanSender *SpanSender) Interceptor(span map[string]interface{}) bool {
	log.Debug("span spanSender interceptor")
	if span, err := spanSender.makeSpan(span); err == nil {
		// send channel
		spanSender.spanMessageCh <- &v1.PSpanMessage{
			Field: &v1.PSpanMessage_Span{
				Span: span,
			},
		}
		// recv the channel status
		select {
		case statusCode := <-spanSender.spanRespCh:
			log.Warn("span send stream is offline statusCode:%d, clear all string/sql/api meta data", statusCode)
			spanSender.cleanAllMetaData()
		case <-time.After(0 * time.Second):
			// do nothing, just go on
		}
	} else {
		log.Warnf("SpanSender::Interceptor return err:%s", err)
	}
	return true
}

func (spanSender *SpanSender) SenderGrpcMetaData(name string, id int32, Type int32,
	params PARAMS_TYPE) {
	config := common.GetConfig()
	conn, err := common.CreateGrpcConnection(config.AgentAddress)
	if err != nil {
		log.Warnf("connect:%s failed. %s", config.AgentAddress, err)
		return
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

				//if value, OK := params["line"]; OK {
				//	line = value
				//}
			}
			// disable line
			apiMeta := v1.PApiMetaData{ApiId: id, ApiInfo: name, Type: _type}

			if _, err = client.RequestApiMetaData(ctx, &apiMeta); err != nil {
				log.Warnf("agentOnline api meta failed %s", err)
			}
		}
		break
	case common.META_STRING:
		{
			metaMeta := v1.PStringMetaData{
				StringId:    id,
				StringValue: name,
			}

			if _, err = client.RequestStringMetaData(ctx, &metaMeta); err != nil {
				log.Warnf("agentOnline api meta failed %s", err)
			}
		}
		break
	case common.META_SQL:
		{
			sqlMeta := v1.PSqlMetaData{
				SqlId: id,
				Sql:   name,
			}

			if _, err = client.RequestSqlMetaData(ctx, &sqlMeta); err != nil {
				log.Warnf("agentOnline api meta failed %s", err)
			}
		}
		break
	default:
		log.Warnf("SenderGrpcMetaData: No such Type:%d", Type)
	}

	log.Debugf("send metaData:type:%d,Id:%d,value:%s para:%v", Type, id, name, params)
}
