package agent

import (
	"github.com/pinpoint-apm/pinpoint-c-agent/collector-agent/common"
	v1 "github.com/pinpoint-apm/pinpoint-c-agent/collector-agent/pinpoint-grpc-idl-go/proto/v1"
	log "github.com/sirupsen/logrus"
	"google.golang.org/grpc/metadata"
)

type ErrorAnalysisFilter struct {
	md metadata.MD
	// only one gocoroutine working on chn, so no needs to lock it
	id int64
}

func createErrorAnalysisFilter(base metadata.MD) *ErrorAnalysisFilter {
	eaf := &ErrorAnalysisFilter{
		md: base,
		id: 0,
	}
	return eaf
}

func (e *ErrorAnalysisFilter) sendExpMetaData(meta *v1.PExceptionMetaData) {
	config := common.GetConfig()
	conn, err := common.CreateGrpcConnection(config.AgentAddress)
	if err != nil {
		log.Warnf("connect:%s failed. %s", config.AgentAddress, err)
		return
	}

	defer conn.Close()
	client := v1.NewMetadataClient(conn)

	ctx, cancel := common.BuildPinpointCtx(config.MetaDataTimeWaitSec, e.md)
	defer cancel()
	result, err := client.RequestExceptionMetaData(ctx, meta)
	if err != nil {
		log.Warnf("RequestExceptionMetaData failed. reason: %v", err)
	}
	log.Debugf("RequestExceptionMetaData %v %v", meta, result)
}

func (e *ErrorAnalysisFilter) parseException(spanEv []*TSpanEvent, exceptions *[]*v1.PException,
	parentErrorId int64, depth int32, parentExp *TExceptionInfo, startTime int64) error {
	for _, ev := range spanEv {
		if ev.ExceptionInfoV2 != nil {
			exp := &v1.PException{
				ExceptionMessage:   ev.ExceptionInfoV2.Message,
				ExceptionClassName: ev.ExceptionInfoV2.ClassName,
				StartTime:          ev.ExceptionInfoV2.StartTime + startTime,
			}
			// TODO, just compare the  message right now
			if parentExp != nil && parentExp.Message == ev.ExceptionInfoV2.Message {
				exp.ExceptionId = parentErrorId
			} else {
				exp.ExceptionId = e.getNewExceptionId()
			}

			exp.ExceptionDepth = depth + 1
			*exceptions = append(*exceptions, exp)
			if len(ev.Calls) > 0 {
				e.parseException(ev.Calls, exceptions, exp.ExceptionId, exp.ExceptionDepth, ev.ExceptionInfoV2, startTime)
			}
		} else {
			if len(ev.Calls) > 0 {
				e.parseException(ev.Calls, exceptions, parentErrorId, depth, parentExp, startTime)
			}
		}
	}
	return nil
}

func (e *ErrorAnalysisFilter) getNewExceptionId() int64 {
	e.id += 1
	return e.id
}

func (e *ErrorAnalysisFilter) scanTSpanTree(span *TSpan) *v1.PExceptionMetaData {
	e_md := &v1.PExceptionMetaData{
		TransactionId: common.TypeV1_String_TransactionId(span.TransactionId),
		SpanId:        span.SpanId,
		UriTemplate:   span.UT,
	}
	var spanExp []*v1.PException
	depth := int32(0)
	startTime := span.GetStartTime()
	if span.ExceptionInfoV2 != nil {
		exp := &v1.PException{
			ExceptionClassName: span.ExceptionInfoV2.ClassName,
			ExceptionMessage:   span.ExceptionInfoV2.Message,
			ExceptionId:        e.getNewExceptionId(),
			ExceptionDepth:     depth,
			StartTime:          span.ExceptionInfoV2.StartTime + startTime,
		}
		spanExp = append(spanExp, exp)
		e.parseException(span.Calls, &spanExp, exp.ExceptionId, depth, span.ExceptionInfoV2, startTime)
	} else {
		e.parseException(span.Calls, &spanExp, 0, depth, nil, startTime)
	}

	e_md.Exceptions = spanExp

	return e_md
}

func (e *ErrorAnalysisFilter) Interceptor(span *TSpan) bool {
	if span.ErrorMarked == 1 {
		// parse the error
		meta := e.scanTSpanTree(span)
		// plan one: send once
		go e.sendExpMetaData(meta)
	} else {
		log.Debugf("ErrorAnalysisFilter Interceptor, not exception mark")
	}
	return true
}
