package agent

import (
	"context"
	"encoding/json"
	"testing"

	"github.com/pinpoint-apm/pinpoint-c-agent/collector-agent/common"
	"google.golang.org/grpc/metadata"
)

func TestGetAgentInfo(t *testing.T) {
	spanMap := &TSpan{
		AppId:           "sfdaefe",
		AppName:         "sfdaefe",
		AppServerTypeV2: 23412,

		TransactionId: "234123424^41234^2333",
	}

	router := CreateAgentRouter(common.CreateTestConfig())

	id, name, ft, startTime, err := router.GetAgentInfo(spanMap)

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
	config := common.CreateTestConfig()
	ea := createErrorAnalysisFilter(context.Background(), md, config, config.LogEntry)

	meta := ea.scanTSpanTree(&tSpan)
	if len(meta.Exceptions) == 0 {
		t.Errorf("empty exception %v", meta.Exceptions)
	}
}

func TestTspan(t *testing.T) {
	msg := `{":E":1,":FT":1500,":S":1728466073494,"appid":"cd.dev.test.run","appname":"cd.dev.test.php","client":"localhost","event":[{":E":0,":S":1,":depth":1,":seq":0,"name":"SimplePHP\\MessageHandler::handle_message_in_kafka","stp":"1501"}],"name":"RdKafka\\KafkaConsumer::consume","server":"localhost","sid":"2057154795","stp":"1500","tid":"cd.dev.test.run^1728466064272^0","uri":"abc","EXP":"xxxxx","ERR":{"msg":"asbc","file":"files","line":233}} `
	var tspan TSpan

	err := json.Unmarshal([]byte(msg), &tspan)
	if err != nil {
		t.Error(err)
	}

	if tspan.GetStartTime() != 1728466073494 {
		t.Error(tspan.GetStartTime())
	}

	if tspan.GetAppName() != "cd.dev.test.php" {
		t.Error(tspan.GetAppName())

	}

	if tspan.SpanId != 2057154795 {
		t.Error(tspan.SpanId)
	}

	if tspan.ServerType != 1500 {
		t.Error(tspan.ServerType)
	}

	if tspan.ExceptionInfo != "xxxxx" {
		t.Error(tspan.ExceptionInfo)
	}

	if tspan.ErrorInfo == nil {
		t.Error("no error info")
	}

	if len(tspan.Follows) == 0 {
		t.Error("no calls")
	}

	// for _, ev := range tspan.Calls {
	// evCalls := ev.Calls
	// if len(evCalls) == 0 {
	// 	t.Error("no calls")
	// }
	// if evCalls[0].Name == "app\\AppDate::abc" {
	// 	t.Error("calls no name")
	// }

	// }

}
