package agent

import (
	"math"
	"time"

	"github.com/pinpoint-apm/pinpoint-c-agent/collector-agent/common"
)

type RequestCounter struct {
	counter                                    [4]uint16
	reqProfileLastTime, reqTop1LastTime, CTime int64
	max, total, times                          uint32
	config                                     *common.Config
}

func createRequestCounter(config *common.Config) *RequestCounter {
	return &RequestCounter{
		config: config,
		CTime:  time.Now().Unix(),
	}
}

func (*RequestCounter) Stop() {

}

// exp in seconds
func (*RequestCounter) getReqLevel(exp uint32) int32 {
	if exp <= 1 {
		return 0
	} else if exp <= 3 {
		return 1
	} else if exp <= 5 {
		return 2
	} else {
		return 3
	}
}

func (reqProf *RequestCounter) updateReqTimeProfile(exp uint32) {
	if reqProf.reqProfileLastTime != reqProf.CTime {
		for i := range reqProf.counter {
			reqProf.counter[i] = 0
		}
	}

	reqProf.counter[reqProf.getReqLevel(exp)] += 1
	reqProf.reqProfileLastTime = reqProf.CTime
}

func (reqProf *RequestCounter) updateReqTop1TimeSummary(exp uint32) {

	if reqProf.CTime >= (reqProf.reqTop1LastTime + int64(reqProf.config.StatInterval) + 1) { // reset response time summary
		reqProf.reqTop1LastTime = reqProf.CTime
		reqProf.total = 0
		reqProf.times = 0
		reqProf.max = 0
	}

	if reqProf.max < exp {
		reqProf.max = exp
	}

	reqProf.total += exp
	reqProf.times += 1
}

func (reqProf *RequestCounter) GetMaxAvg() (max, avg uint32) {
	if time.Now().Unix() < (reqProf.reqTop1LastTime+int64(reqProf.config.StatInterval)+1) && reqProf.times > 0 {
		return reqProf.max, reqProf.total / reqProf.times
	} else {
		return 0, 0
	}
}

func (reqProf *RequestCounter) GetReqTimeProfiler() [4]uint16 {
	now := time.Now().Unix()
	if now < reqProf.reqProfileLastTime+2 {
		return reqProf.counter
	} else {
		return [4]uint16{0, 0, 0, 0}
	}
}

func (reqProf *RequestCounter) Interceptor(span *TSpan) bool {

	if span.LocalAsyncId != nil {
		return true
	}

	reqProf.CTime = time.Now().Unix()
	elapsed := span.GetElapsedTime()

	reqProf.updateReqTop1TimeSummary(uint32(elapsed))
	exp := uint32(math.Ceil(float64(elapsed) * 1.0 / 1000.0))
	reqProf.updateReqTimeProfile(exp)

	return true
}
