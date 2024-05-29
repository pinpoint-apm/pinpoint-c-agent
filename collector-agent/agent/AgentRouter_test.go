package agent

import (
	"encoding/json"
	"testing"

	"google.golang.org/grpc/metadata"
)

func TestGetAgentInfo(t *testing.T) {
	// spanMap := map[string]interface{}{
	// 	"appid":   "sfdaefe",
	// 	"appname": "sfdaefe",
	// 	"FT":      float64(23412),
	// 	"tid":     "234123424^41234^2333",
	// }

	spanMap := &TSpan{
		AppId:           "sfdaefe",
		AppName:         "sfdaefe",
		AppServerTypeV2: 23412,

		TransactionId: "234123424^41234^2333",
	}

	id, name, ft, startTime, err := GetAgentInfo(spanMap)

	if id != "sfdaefe" && name != "sfdaefe" && ft != 23412 && startTime != "234123424" && err != nil {
		t.Error(spanMap)
	}

}

func Test_EASpan(t *testing.T) {
	msg := `{":E":3,":FT":1700,":S":1714454599243,"EA":1,"ERR":{"file":"FastAPIRequestPlugin","line":0,"msg":"status_code:500 INTERNAL SERVER ERROR"},"UT":"/test_exception_in_Chain","appid":"cd.dev.test.flask","appname":"cd.dev.test.py","calls":[{":E":0,":S":1,"EXP":"abcd","calls":[{":E":0,":S":0,"EXP":"abc","name":"call_exp_01","stp":"1701"},{":E":0,":S":0,"EXP":"abcd","name":"call_exp_02","stp":"1701"}],"name":"main","stp":"1701"}],"client":"172.24.0.1","clues":["206:GET","46:500 INTERNAL SERVER ERROR"],"name":"BaseFlaskrequest","server":"localhost:8184","sid":"212686650","stp":"1700","tid":"cd.dev.test.flask^1714448478218^1205","uri":"/test_exception_in_Chain","EXP_V2":{"M":"asgdf","C":"xxxx",":S":2}}`
	var tSpan TSpan

	err := json.Unmarshal([]byte(msg), &tSpan)
	if err != nil {
		t.Error(err)
	}

	if tSpan.ErrorMarked != 1 {
		t.Errorf("EA missed")
	}

	md := metadata.New(map[string]string{
		"test":  "2",
		"test2": "string",
	})
	ea := createErrorAnalysisFilter(md)

	meta := ea.scanTSpanTree(&tSpan)
	if len(meta.Exceptions) == 0 {
		t.Errorf("empty exception %v", meta.Exceptions)
	}
}

func TestTspan(t *testing.T) {
	msg := `{"E":1,"FT":1500,":FT":1500,"ptype":"1500","pname":"abc_d","psid":"23563","NP":"t=1617083759.535 D=0.000","S":1617083759798,"appid":"app-2",":appid":"app-2",
	":appname":"APP-2","appname":"APP-2","calls":[{"E":1,"calls":[{"E":1,"S":0,"clues":["-1:input parameters","14:return value"],"name":"abc"}],"S":0,"clues":["-1:input parameters","14:return value"],"name":"app\\AppDate::abc","SQL":"select* from abc"}],"client":"10.34.135.145","clues":["46:200"],"name":"PHP Request: fpm-fcgi","server":"10.34.130.152:8000","sid":"726125302","stp":"1500","tid":"app-2^1617083747^5506","uri":"/index.php?type=get_date","Ah":"123.35.36.3/host","EXP":"exp","ERR":{"msg":"error_msg","file":"file.cc","line":123}}`
	var tspan TSpan

	err := json.Unmarshal([]byte(msg), &tspan)
	if err != nil {
		t.Error(err)
	}

	if tspan.GetStartTime() != 1617083759798 {
		t.Error(tspan.GetStartTime())
	}

	if tspan.GetAppName() != "APP-2" {
		t.Error(tspan.GetAppName())

	}

	if tspan.SpanId != 726125302 {
		t.Error(tspan.SpanId)
	}

	if tspan.ServerType != 1500 {
		t.Error(tspan.ServerType)
	}

	if tspan.ExceptionInfo != "exp" {
		t.Error(tspan.ExceptionInfo)
	}

	if tspan.ErrorInfo == nil {
		t.Error("no error info")
	}

	if len(tspan.Calls) == 0 {
		t.Error("no calls")
	}

	for _, ev := range tspan.Calls {
		evCalls := ev.Calls
		if len(evCalls) == 0 {
			t.Error("no calls")
		}
		if evCalls[0].Name == "app\\AppDate::abc" {
			t.Error("calls no name")
		}

	}

}
