package agent

import (
	"sync"
	"time"

	v1 "github.com/pinpoint-apm/pinpoint-c-agent/collector-agent/pinpoint-grpc-idl-go/proto/v1"
)

const bucketVersion = 0
const histogramSize = 8

type uriStatHistogram struct {
	Total              int64
	Max                int64
	TimestampHistogram [histogramSize]int32
}

func (ust *uriStatHistogram) Update(span *TSpan) {
	elapseTime := span.GetElapsedTime()
	ust.Total += int64(elapseTime)

	if int64(elapseTime) > ust.Max {
		ust.Max = int64(elapseTime)
	}
	ust.TimestampHistogram[span.FindHistogramLevel()] += 1
}

func (ust *uriStatHistogram) ToUriHistogram() *v1.PUriHistogram {
	pbUriHistogram := &v1.PUriHistogram{
		Total:     ust.Total,
		Max:       ust.Max,
		Histogram: ust.TimestampHistogram[:],
	}
	return pbUriHistogram
}

type statHistograms struct {
	TotalHistogram  uriStatHistogram
	FailedHistogram uriStatHistogram
}

func (st *statHistograms) Update(span *TSpan) {
	st.TotalHistogram.Update(span)
	if span.IsFailed() {
		st.FailedHistogram.Update(span)
	}
}

type UrlTemplateReport struct {
	uriMap        map[string]*statHistograms
	BucketVersion int32
	mu            sync.Mutex
}

func (utr *UrlTemplateReport) Interceptor(span *TSpan) bool {
	if len(span.UT) > 0 {
		// found uri templated
		utr.updateUriSnapshot(span)
	}
	return true
}

func (utr *UrlTemplateReport) updateUriSnapshot(span *TSpan) {
	utr.mu.Lock()
	defer utr.mu.Unlock()
	ut := span.UT
	var st *statHistograms
	var ok bool
	if st, ok = utr.uriMap[ut]; !ok {
		st = &statHistograms{}
		utr.uriMap[ut] = st
	}
	st.Update(span)
}

func (utr *UrlTemplateReport) MoveUtReport() *v1.PStatMessage {
	utr.mu.Lock()
	defer utr.mu.Unlock()

	agentUriStat := &v1.PAgentUriStat{
		BucketVersion: int32(utr.BucketVersion),
	}

	for url, st := range utr.uriMap {
		eachUriStat := &v1.PEachUriStat{
			Uri:             url,
			TotalHistogram:  st.TotalHistogram.ToUriHistogram(),
			FailedHistogram: st.FailedHistogram.ToUriHistogram(),
			Timestamp:       time.Now().UnixMilli(),
		}
		agentUriStat.EachUriStat = append(agentUriStat.EachUriStat, eachUriStat)
	}
	//note: create a new one
	utr.uriMap = make(map[string]*statHistograms)
	pbStat := &v1.PStatMessage{
		Field: &v1.PStatMessage_AgentUriStat{
			AgentUriStat: agentUriStat,
		},
	}

	return pbStat
}

func CreateUrlTemplateReport() *UrlTemplateReport {
	ut := &UrlTemplateReport{
		uriMap:        make(map[string]*statHistograms),
		BucketVersion: bucketVersion,
	}
	return ut
}
