package agent

import (
	"testing"

	"google.golang.org/grpc/metadata"
)

func TestErrorAnalysisFilter_Interceptor(t *testing.T) {
	md := metadata.New(map[string]string{
		"test":  "2",
		"test2": "string",
	})
	ea := createErrorAnalysisFilter(md)

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
			Calls: []*TSpanEvent{
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
			Calls: []*TSpanEvent{
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
