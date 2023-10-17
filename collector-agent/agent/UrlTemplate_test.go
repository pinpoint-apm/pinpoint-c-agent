package agent

import (
	"testing"

	"github.com/stretchr/testify/assert"
)

func TestUrlTemplateReport(t *testing.T) {
	spans := []TSpan{
		{
			UT:          "/hello",
			Uri:         "/hello",
			ElapsedTime: 32,
		},
		{
			UT:          "/hello",
			Uri:         "/hello",
			ElapsedTime: 320,
		},
		{
			UT:          "/hello",
			Uri:         "/hello",
			ElapsedTime: 3200,
		},
		{
			UT:            "/hello_exp",
			Uri:           "/hello",
			ElapsedTime:   32000,
			ExceptionInfo: "exp",
		},
	}

	ut := CreateUrlTemplateReport()
	for _, span := range spans {
		ut.Interceptor(&span)
	}

	if len(ut.uriMap) < 2 {
		t.Log(len(ut.uriMap))
	}

	pbStatMessage := ut.MoveUtReprot()
	t.Log(pbStatMessage)
	assert.NotEqual(t, pbStatMessage.GetAgentUriStat(), nil, "GetAgentUriStat")

	pbUriStat := pbStatMessage.GetAgentUriStat()

	assert.Equal(t, pbUriStat.GetBucketVersion(), int32(0), "GetBucketVersion")

	eachUriStat := pbUriStat.GetEachUriStat()

	assert.Equal(t, len(eachUriStat), 2, "len(eachUriStat)")

	assert.NotEqual(t, eachUriStat[0].GetFailedHistogram(), nil, "GetFailedHistogram")
	assert.NotEqual(t, eachUriStat[0].GetTotalHistogram(), nil, "GetTotalHistogram")
	totalHis := eachUriStat[0].GetTotalHistogram()
	assert.Equal(t, len(totalHis.GetHistogram()), histogramSize, "len(totalHis.GetHistogram())")

	assert.Equal(t, totalHis.Max, int64(3200), "totalHis.Max")

}
