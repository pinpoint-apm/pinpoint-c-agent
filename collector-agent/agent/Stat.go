package agent

import (
	"time"

	"github.com/pinpoint-apm/pinpoint-c-agent/collector-agent/common"
	v1 "github.com/pinpoint-apm/pinpoint-c-agent/collector-agent/pinpoint-grpc-idl-go/proto/v1"

	"github.com/shirou/gopsutil/cpu"
	"github.com/shirou/gopsutil/v3/mem"
)

//type convert func(int) string

type GetMaxAvg func() (max, avg uint32)
type GetReqTimeCounter func() [4]uint16

func CollectPStateMessage(getMaxAvr GetMaxAvg, getReqTimeCounter GetReqTimeCounter) *v1.PStatMessage {
	config := common.GetConfig()
	max, avg := getMaxAvr()
	responseTime := v1.PResponseTime{
		Max: int64(max),
		Avg: int64(avg),
	}

	v, _ := mem.VirtualMemory()

	jvmGc := v1.PJvmGc{
		Type:                 v1.PJvmGcType_JVM_GC_TYPE_PARALLEL,
		JvmMemoryHeapUsed:    int64(v.Used),
		JvmMemoryHeapMax:     int64(v.Total),
		JvmMemoryNonHeapUsed: int64(v.Buffers),
		JvmMemoryNonHeapMax:  int64(v.Cached),
		JvmGcOldCount:        0,
		JvmGcOldTime:         0,
		JvmGcDetailed:        &v1.PJvmGcDetailed{},
	}
	// cpu.Percent calcuate cpu in config.StatInterval
	totalPer, _ := cpu.Percent(config.StatInterval*time.Second, false)
	totalCpuUsage := totalPer[0] / 100
	cpuload := v1.PCpuLoad{
		SystemCpuLoad: totalCpuUsage,
		JvmCpuLoad:    totalCpuUsage,
	}
	var activeTraceCount []int32
	for _, value := range getReqTimeCounter() {
		activeTraceCount = append(activeTraceCount, int32(value))
	}

	agentStat := v1.PAgentStat{
		ResponseTime:    &responseTime,
		Gc:              &jvmGc,
		CollectInterval: int64(config.StatInterval),
		Timestamp:       int64(time.Now().UnixNano() / int64(time.Millisecond)),
		CpuLoad:         &cpuload,
		Transaction:     &v1.PTransaction{},
		ActiveTrace: &v1.PActiveTrace{
			Histogram: &v1.PActiveTraceHistogram{
				Version:             1,
				HistogramSchemaType: 2, //NORMAL SCHEMA
				ActiveTraceCount:    activeTraceCount,
			},
		},
		DataSourceList: nil,
		Deadlock:       nil,
		FileDescriptor: nil,
		DirectBuffer:   nil,
		Metadata:       "",
	}

	pStateAgentStat := v1.PStatMessage_AgentStat{
		AgentStat: &agentStat,
	}

	sateMessage := v1.PStatMessage{
		Field: &pStateAgentStat,
	}

	return &sateMessage
}
