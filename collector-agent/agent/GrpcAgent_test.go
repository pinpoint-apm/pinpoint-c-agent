package agent

import (
	"fmt"
	"strconv"
	"testing"
	"time"

	"github.com/pinpoint-apm/pinpoint-c-agent/collector-agent/common"
)

/**
GRPC_TRACE="all"
GRPC_VERBOSITY="DEBUG"
GODEBUG="http2debug=2"
GRPC_GO_LOG_VERBOSITY_LEVEL=99
GRPC_GO_LOG_SEVERITY_LEVEL=info
unset GODEBUG
*/

func Test_GrpcAgentBackgroundTask(t *testing.T) {
	config := common.CreateTestConfig()
	startTime := time.Now().UTC().UnixMicro()
	agent := CreateGrpcAgent("cd.dev.test.ci", "cd.dev.test", 1500, 1, strconv.FormatInt(startTime, 10), config)
	t.Logf("wait 10sec")
	time.Sleep(5 * time.Second)
	agent.Stop()
}

func Test_async_call_map(t *testing.T) {
	config := common.CreateTestConfig()
	startTime := time.Now().Unix()
	agent := CreateGrpcAgent("cd.dev.test.ci", "cd.dev.test", 1500, 1, strconv.FormatInt(startTime, 10), config)
	agent.StartServe()
	tid := fmt.Sprintf("cd.dev.test.ci^%d^%d", startTime, 1)

	span := &TSpan{AppServerType: 1500, AppServerTypeV2: 1500, StartTimeV2: startTime, ElapsedTime: 100, ElapsedTimeV2: 103, AppId: "cd.dev.test.ci", AppIdV2: "cd.dev.test.ci", AppName: "cd.dev.test", AppNameV2: "cd.dev.test", SpanName: "test-ci-main", SpanId: 454525, ServerType: 1500, TransactionId: tid, Uri: "/", RemoteAddr: "15.36.89.23", EndPoint: "localhost:5265",
		Follows: []*TSpanEvent{
			{
				Name:        "test-1",
				Sequence:    0,
				ServiceType: 1501,
				Depth:       1,
				// AsyId:        1,
				StartElapsed: int32(time.Now().UTC().UnixMicro()) + 4,
				EndElapsed:   10,
			},
			{
				Name:         "test-3",
				Sequence:     2,
				Depth:        2,
				ServiceType:  1501,
				AsyId:        2578,
				StartElapsed: int32(time.Now().UTC().UnixMicro()) + 30,
				EndElapsed:   20,
			},
			{
				Name:         "test-2",
				Sequence:     1,
				Depth:        2,
				ServiceType:  1501,
				AsyId:        1589,
				StartElapsed: int32(time.Now().UTC().UnixMicro()) + 30,
				EndElapsed:   20,
			}},
	}

	chunk := &TSpan{
		TransactionId: tid,
		SpanId:        454525,
		StartTime:     startTime + 7898,
		AppServerType: 1500,
		EndPoint:      "localhost:5265",
		LocalAsyncId:  &TAsyncId{AsyncId: 1589, Sequence: 2},
		Follows: []*TSpanEvent{
			{
				Name:        "thread_func",
				Depth:       1,
				EndElapsed:  300,
				ServiceType: 100,
			},
			{
				Name:        "test-7",
				Depth:       2,
				Sequence:    1,
				EndElapsed:  5000,
				ServiceType: 9054,
			},
		},
	}

	chunk2 := &TSpan{
		TransactionId: tid,
		SpanId:        454525,
		StartTime:     startTime + 7898,
		AppServerType: 1500,
		EndPoint:      "localhost:5265",
		LocalAsyncId:  &TAsyncId{AsyncId: 2578, Sequence: 1},
		Follows: []*TSpanEvent{
			{
				Name:        "thread_fun_01",
				Depth:       1,
				EndElapsed:  200,
				ServiceType: 100,
			},
			{
				Name:        "test-7-chunk2",
				Depth:       2,
				Sequence:    1,
				EndElapsed:  2000,
				ServiceType: 9054,
			},
		},
	}

	agent.SendSpan(chunk)
	agent.SendSpan(span)

	// t.Log("sleep 10sec")
	time.Sleep(5 * time.Second)
	agent.SendSpan(chunk2)
	time.Sleep(5 * time.Second)
	agent.Stop()
	// t.Error("---")
	t.Log("every thing is fine")
}
