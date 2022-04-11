package agent

import (
	"math"
	"time"

	"github.com/pinpoint-apm/pinpoint-c-agent/collector-agent/common"

	log "github.com/sirupsen/logrus"
)

type RequestProfiler struct {
	reqProfiler                                [4]uint16
	reqProfileLastTime, reqTop1LastTime, CTime int64
	max, total, times                          uint32
}

// exp in seconds
func (self *RequestProfiler) getReqLevel(exp uint32) int32 {
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

func (self *RequestProfiler) updateReqTimeProfile(exp uint32) {
	if self.reqProfileLastTime != self.CTime {
		for i, _ := range self.reqProfiler {
			self.reqProfiler[i] = 0
		}
	}

	self.reqProfiler[self.getReqLevel(exp)] += 1
	self.reqProfileLastTime = self.CTime
}

func (self *RequestProfiler) updateReqTop1TimeSummary(exp uint32) {
	config := common.GetConfig()
	if self.CTime >= (self.reqTop1LastTime + int64(config.StatInterval) + 1) { // reset response time summary
		self.reqTop1LastTime = self.CTime
		self.total = 0
		self.times = 0
		self.max = 0
	}

	if self.max < exp {
		self.max = exp
	}

	self.total += exp
	self.times += 1
}

func (self *RequestProfiler) GetMaxAvg() (max, avg uint32) {
	config := common.GetConfig()
	if time.Now().Unix() < (self.reqTop1LastTime + int64(config.StatInterval) + 1) {
		return self.max, self.total / self.times
	} else {
		return 0, 0
	}
}

func (self *RequestProfiler) GetReqTimeProfiler() [4]uint16 {
	now := time.Now().Unix()
	if now < self.reqProfileLastTime+2 {
		return self.reqProfiler
	} else {
		return [4]uint16{0, 0, 0, 0}
	}
}

func (self *RequestProfiler) Interceptor(span map[string]interface{}) bool {
	if exp, OK := span["E"]; OK {
		self.CTime = time.Now().Unix()
		if iexp, OK := exp.(float64); OK {

			self.updateReqTop1TimeSummary(uint32(iexp))
			exp := uint32(math.Ceil(iexp / 1000.0))
			self.updateReqTimeProfile(exp)

		} else {
			log.Warnf("Convert exp to float64 failed.Reason %s", exp)
		}
	}
	return true
}
