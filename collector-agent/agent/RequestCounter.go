package agent

import (
	"math"
	"time"

	"github.com/pinpoint-apm/pinpoint-c-agent/collector-agent/common"
)

type RequestProfiler struct {
	reqProfiler                                [4]uint16
	reqProfileLastTime, reqTop1LastTime, CTime int64
	max, total, times                          uint32
}

// exp in seconds
func (*RequestProfiler) getReqLevel(exp uint32) int32 {
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

func (reqProf *RequestProfiler) updateReqTimeProfile(exp uint32) {
	if reqProf.reqProfileLastTime != reqProf.CTime {
		for i := range reqProf.reqProfiler {
			reqProf.reqProfiler[i] = 0
		}
	}

	reqProf.reqProfiler[reqProf.getReqLevel(exp)] += 1
	reqProf.reqProfileLastTime = reqProf.CTime
}

func (reqProf *RequestProfiler) updateReqTop1TimeSummary(exp uint32) {
	config := common.GetConfig()
	if reqProf.CTime >= (reqProf.reqTop1LastTime + int64(config.StatInterval) + 1) { // reset response time summary
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

func (reqProf *RequestProfiler) GetMaxAvg() (max, avg uint32) {
	config := common.GetConfig()
	if time.Now().Unix() < (reqProf.reqTop1LastTime + int64(config.StatInterval) + 1) {
		return reqProf.max, reqProf.total / reqProf.times
	} else {
		return 0, 0
	}
}

func (reqProf *RequestProfiler) GetReqTimeProfiler() [4]uint16 {
	now := time.Now().Unix()
	if now < reqProf.reqProfileLastTime+2 {
		return reqProf.reqProfiler
	} else {
		return [4]uint16{0, 0, 0, 0}
	}
}

func (reqProf *RequestProfiler) Interceptor(span *TSpan) bool {
	reqProf.CTime = time.Now().Unix()
	elapsed := span.GetElapsedTime()

	reqProf.updateReqTop1TimeSummary(uint32(elapsed))
	exp := uint32(math.Ceil(float64(elapsed) * 1.0 / 1000.0))
	reqProf.updateReqTimeProfile(exp)

	return true
}
