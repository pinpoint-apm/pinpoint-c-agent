package agent

import (
	"sync"
	"time"

	v1 "github.com/pinpoint-apm/pinpoint-c-agent/collector-agent/pinpoint-grpc-idl/proto/v1"
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

func (ust *uriStatHistogram) ToUriHistogrm() *v1.PUriHistogram {
	pbUriHistogram := &v1.PUriHistogram{
		Total:     ust.Total,
		Max:       ust.Max,
		Histogram: ust.TimestampHistogram[:],
	}
	return pbUriHistogram
}

type statHisograms struct {
	TotalHistogram  uriStatHistogram
	FailedHistogram uriStatHistogram
}

func (st *statHisograms) Update(span *TSpan) {
	st.TotalHistogram.Update(span)
	if span.IsFailed() {
		st.FailedHistogram.Update(span)
	}
}

type UrlTemplateReport struct {
	uriMap        map[string]*statHisograms
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
	var st *statHisograms
	var ok bool
	if st, ok = utr.uriMap[ut]; !ok {
		st = &statHisograms{}
		utr.uriMap[ut] = st
	}
	st.Update(span)
}

func (utr *UrlTemplateReport) MoveUtReprot() *v1.PStatMessage {
	utr.mu.Lock()
	defer utr.mu.Unlock()

	agentUriStat := &v1.PAgentUriStat{
		BucketVersion: int32(utr.BucketVersion),
	}

	for url, st := range utr.uriMap {
		eachUriStat := &v1.PEachUriStat{
			Uri:             url,
			TotalHistogram:  st.TotalHistogram.ToUriHistogrm(),
			FailedHistogram: st.FailedHistogram.ToUriHistogrm(),
			Timestamp:       time.Now().UnixMilli(),
		}
		agentUriStat.EachUriStat = append(agentUriStat.EachUriStat, eachUriStat)
	}
	//note: create a new one
	utr.uriMap = make(map[string]*statHisograms)
	pbStat := &v1.PStatMessage{
		Field: &v1.PStatMessage_AgentUriStat{
			AgentUriStat: agentUriStat,
		},
	}

	return pbStat
}

func CreateUrlTemplateReport() *UrlTemplateReport {
	ut := &UrlTemplateReport{
		uriMap:        make(map[string]*statHisograms),
		BucketVersion: bucketVersion,
	}
	return ut
}
