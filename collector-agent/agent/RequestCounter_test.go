package agent

import (
	"CollectorAgent/common"
	"testing"
	"time"
)

func TestRequestProfiler_Interceptor(t *testing.T) {
	profiler := RequestProfiler{}
	config := common.GetConfig()
	config.StatInterval = 30
	spans := []map[string]interface{}{
		{"E": 10.0},
		{"E": 2568.0},
		{"E": 100.0},
		{"E": 520.0},
		{"E": 0.0},
	}
	for _, v := range spans {
		profiler.Interceptor(v)
	}

	time.Sleep(1 * time.Second)
	profiler.Interceptor(spans[2])

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
	spanMap := map[string]interface{}{
		"E": float64(234),
	}
	req := RequestProfiler{}
	for i := 0; i < b.N; i++ {
		req.Interceptor(spanMap)
	}
}
