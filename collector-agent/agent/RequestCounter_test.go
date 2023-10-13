package agent

import (
	"testing"
	"time"

	"github.com/pinpoint-apm/pinpoint-c-agent/collector-agent/common"
)

func TestRequestProfiler_Interceptor(t *testing.T) {
	profiler := RequestProfiler{}
	config := common.GetConfig()
	config.StatInterval = 30
	spans := []TSpan{
		{ElapsedTime: 10},
		{ElapsedTime: 2568},
		{ElapsedTime: 100},
		{ElapsedTime: 520},
		{ElapsedTime: 0},
	}
	for _, v := range spans {
		profiler.Interceptor(&v)
	}

	time.Sleep(1 * time.Second)
	profiler.Interceptor(&spans[2])

	targ := profiler.GetReqTimeProfiler()
	if targ[0] != 1 {
		t.Fail()
	}

	max, avg := profiler.GetMaxAvg()
	//fmt.Print(max)
	//fmt.Print(avg)
	if max != 2568 || avg != 549 {
		t.Fail()
	}
}

func BenchmarkRequestProfiler_Interceptor(b *testing.B) {
	spanMap := TSpan{
		ElapsedTime: 234,
	}
	req := RequestProfiler{}
	for i := 0; i < b.N; i++ {
		req.Interceptor(&spanMap)
	}
}
