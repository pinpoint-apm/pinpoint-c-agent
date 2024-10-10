package agent

import (
	"context"
	"testing"

	"github.com/pinpoint-apm/pinpoint-c-agent/collector-agent/common"
	"google.golang.org/grpc/metadata"
)

func TestErrorAnalysisFilter_Interceptor(t *testing.T) {
	md := metadata.New(map[string]string{
		"test":  "2",
		"test2": "string",
	})
	config := common.CreateTestConfig()
	ea := createErrorAnalysisFilter(context.Background(), md, config, config.LogEntry)

	cases := []*TSpan{
		{
			ErrorMarked: 0,
		},
		{
			ErrorMarked: 1,
		},
		{
			ErrorMarked:   1,
			ExceptionInfo: "abc",
			Follows: []*TSpanEvent{
				{
					ExceptionInfo: "abc",
				},
				{
					ExceptionInfo: "",
				},
			},
		},
		{
			ErrorMarked:   0,
			ExceptionInfo: "abc",
			Follows: []*TSpanEvent{
				{
					ExceptionInfo: "abc",
				},
				{
					ExceptionInfo: "",
				},
			},
		},
	}

	for _, c := range cases {
		ea.Interceptor(c)
	}

}
