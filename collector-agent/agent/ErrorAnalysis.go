package agent

import (
	"context"
	"sync"

	"github.com/pinpoint-apm/pinpoint-c-agent/collector-agent/common"
	v1 "github.com/pinpoint-apm/pinpoint-c-agent/collector-agent/pinpoint-grpc-idl-go/proto/v1"
	"github.com/sirupsen/logrus"
	"google.golang.org/grpc/metadata"
)

type ErrorAnalysisFilter struct {
	md     metadata.MD
	id     int64
	config *common.Config
	log    *logrus.Entry
	wg     sync.WaitGroup
	ctx    context.Context
}

func createErrorAnalysisFilter(ctx context.Context, base metadata.MD, config *common.Config, entry *logrus.Entry) *ErrorAnalysisFilter {
	return &ErrorAnalysisFilter{
		md:     base,
		id:     0,
		config: config,
		log:    entry,
		ctx:    ctx,
	}
}

func (e *ErrorAnalysisFilter) Stop() {}

func (e *ErrorAnalysisFilter) sendExpMetaData(meta *v1.PExceptionMetaData) {
	defer e.wg.Done()

	conn, err := e.config.CreateGrpcConnection(e.ctx, e.config.User.AgentAddress)
	if err != nil {
		e.log.Warnf("connect:%s failed. %s", e.config.User.AgentAddress, err)
		return
	}

	defer conn.Close()
	client := v1.NewMetadataClient(conn)

	ctx, cancel := common.BuildMdContextWithTimeout(e.config.MetaDataTimeWait, e.md)
	defer cancel()
	result, err := client.RequestExceptionMetaData(ctx, meta)
	if err != nil {
		e.log.Warnf("RequestExceptionMetaData failed. reason: %v", err)
	}
	e.log.Debugf("RequestExceptionMetaData %v %v", meta, result)
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
			// TODO
			// if len(ev.Calls) > 0 {
			// 	e.parseException(ev.Calls, exceptions, exp.ExceptionId, exp.ExceptionDepth, ev.ExceptionInfoV2, startTime)
			// }
		} else {
			// TODO
			// if len(ev.Calls) > 0 {
			// 	e.parseException(ev.Calls, exceptions, parentErrorId, depth, parentExp, startTime)
			// }
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
		e.parseException(span.Follows, &spanExp, exp.ExceptionId, depth, span.ExceptionInfoV2, startTime)
	} else {
		e.parseException(span.Follows, &spanExp, 0, depth, nil, startTime)
	}

	e_md.Exceptions = spanExp

	return e_md
}

func (e *ErrorAnalysisFilter) Interceptor(span *TSpan) bool {
	if span.ErrorMarked == 1 {
		meta := e.scanTSpanTree(span)
		e.wg.Add(1)
		go e.sendExpMetaData(meta)
	} else {
		e.log.Debugf("ErrorAnalysisFilter Interceptor, not exception mark")
	}
	return true
}
