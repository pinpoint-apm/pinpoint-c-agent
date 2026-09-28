package agent

import (
	"math"
	"sync"
	"time"

	"github.com/pinpoint-apm/pinpoint-c-agent/collector-agent/common"
)

type RequestCounter struct {
	// mu guards all mutable statistics below. The span consumer goroutine
	// writes them (Interceptor) while the stat/command/cleanup goroutines read
	// them (GetMaxAvg/GetReqTimeProfiler/GetLastBusyTime). Without this lock
	// those accesses race (reproduced with `go test -race`).
	mu                                         sync.Mutex
	counter                                    [4]int32
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

	// StatInterval is a time.Duration; convert to seconds before adding to a
	// Unix timestamp, otherwise the window would never reset as configured.
	statIntervalSec := int64(reqProf.config.StatInterval / time.Second)
	if reqProf.CTime >= (reqProf.reqTop1LastTime + statIntervalSec + 1) { // reset response time summary
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
	reqProf.mu.Lock()
	defer reqProf.mu.Unlock()
	statIntervalSec := int64(reqProf.config.StatInterval / time.Second)
	if time.Now().Unix() < (reqProf.reqTop1LastTime+statIntervalSec+1) && reqProf.times > 0 {
		return reqProf.max, reqProf.total / reqProf.times
	} else {
		return 0, 0
	}
}

func (reqProf *RequestCounter) GetReqTimeProfiler() [4]int32 {
	reqProf.mu.Lock()
	defer reqProf.mu.Unlock()
	now := time.Now().Unix()
	if now < reqProf.reqProfileLastTime+2 {
		return reqProf.counter
	} else {
		return [4]int32{0, 0, 0, 0}
	}
}

// GetLastBusyTime returns the last time a span was observed (Unix seconds).
// It is read by the router's cleanup goroutine, so it must take the lock.
func (reqProf *RequestCounter) GetLastBusyTime() int64 {
	reqProf.mu.Lock()
	defer reqProf.mu.Unlock()
	return reqProf.CTime
}

func (reqProf *RequestCounter) Interceptor(span *TSpan) bool {

	if span.LocalAsyncId != nil {
		return true
	}

	reqProf.mu.Lock()
	defer reqProf.mu.Unlock()

	reqProf.CTime = time.Now().Unix()
	elapsed := span.GetElapsedTime()

	reqProf.updateReqTop1TimeSummary(uint32(elapsed))
	exp := uint32(math.Ceil(float64(elapsed) * 1.0 / 1000.0))
	reqProf.updateReqTimeProfile(exp)

	return true
}
