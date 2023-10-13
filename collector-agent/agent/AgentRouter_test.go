package agent

import (
	"encoding/json"
	"testing"
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

func TestTspan(t *testing.T) {
	msg := `{"E":1,"FT":1500,":FT":1500,"ptype":1500,"pname":"abc_d","psid":"23563","NP":"t=1617083759.535 D=0.000","S":1617083759798,"appid":"app-2",":appid":"app-2",
	":appname":"APP-2","appname":"APP-2","calls":[{"E":1,"calls":[{"E":1,"S":0,"clues":["-1:input parameters","14:return value"],"name":"abc"}],"S":0,"clues":["-1:input parameters","14:return value"],"name":"app\\AppDate::abc","SQL":"select* from abc"}],"client":"10.34.135.145","clues":["46:200"],"name":"PHP Request: fpm-fcgi","server":"10.34.130.152:8000","sid":"726125302","stp":"1500","tid":"app-2^1617083747^5506","uri":"/index.php?type=get_date","Ah":"123.35.36.3/host","EXP":"exp","ERR":{"msg":"error_msg","file":"file.cc","line":123}}`
	var tspan TSpan

	err := json.Unmarshal([]byte(msg), &tspan)
	if err != nil {
		t.Error(err)
	}

	if tspan.GetStartTime() != 1617083759798 {
		t.Error(tspan.GetStartTime())
	}

	if tspan.GetAppname() != "APP-2" {
		t.Error(tspan.GetAppname())

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

	t.Log(tspan)
}
