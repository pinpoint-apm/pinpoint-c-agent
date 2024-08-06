package server

import (
	"encoding/binary"
	"net"
	"os"
	"strconv"
	"strings"
	"testing"
	"time"

	"github.com/pinpoint-apm/pinpoint-c-agent/collector-agent/common"

	log "github.com/sirupsen/logrus"
)

func launchJsonServer() *SpanServer {
	config := common.GetConfig()

	config.SocketType = "tcp"
	config.Address = "127.0.0.1:8789"
	config.AgentAddress = "dev-pinpoint-01:9991"
	config.StatAddress = "dev-pinpoint-01:9992"
	config.SpanAddress = "dev-pinpoint-01:9993"
	config.WebPorts = 80
	config.LoggerLevel = "DEBUG"
	config.LoggerDir = "/tmp/"

	config.AgentChannelSize = 1000
	config.SpanStreamParallelismSize = 1
	config.AgentReTryTimeout = 10
	config.PingInterval = 5
	config.StatInterval = 5
	config.SpanTimeWait = 10
	config.MetaDataTimeWait = 10
	config.GrpcConTextTimeOut = 5
	config.HostName = "dev-pinpoint"
	config.HostIp = "10.34.135.214"
	config.Pid = 5689
	config.StartTime = 13558755446548
	config.ServerType = 1700
	config.Container = false

	js := SpanServer{}

	return &js
}

func generateValidPacket(msg string) (buf []byte) {
	buffer := make([]byte, 8)
	binary.BigEndian.PutUint32(buffer[0:4], (1))
	binary.BigEndian.PutUint32(buffer[4:8], (uint32(len(msg))))
	buf = append(buffer, msg...)
	return buf
}

func generateUnvaalidPacket01() (buf []byte) {
	buf = make([]byte, 8)
	return buf
}

func genUniqueIdBody() []byte {
	msg := make([]byte, 8)
	binary.BigEndian.PutUint32(msg[0:4], (uint32(2)))
	binary.BigEndian.PutUint32(msg[4:8], (0))
	return msg
}

func generateInvalidPacket02() (buf []byte) {
	buf = make([]byte, 8)
	binary.BigEndian.PutUint32(buf[0:4], (uint32(5220154)))
	binary.BigEndian.PutUint32(buf[4:8], (1))
	return buf
}

func generateInvalidPacket03() (buf []byte) {
	buffer := make([]byte, 8)
	binary.BigEndian.PutUint32(buffer[0:4], uint32(0))
	binary.BigEndian.PutUint32(buffer[4:8], (1))
	return buffer
}

func handleWrite(t *testing.T, conn net.Conn) {
	defer conn.Close()
	conn.Write(genUniqueIdBody())
	readId(t, conn)
	for i := 0; i < 5; i++ {
		_time := time.Now().Unix()
		msg := `{"E":17,"FT":1500,"S":` + strconv.FormatInt(_time, 10) + `,"AppId":"c_test_app_01","Appname":"c_test_name_01","calls":[{"E":7,"S":4,"clues":["-1:I'mthe parameters"],"name":"test_func","stp":"1301"},{"E":2,"S":11,"SQL":"select 1*3;","dst":"localhost:3307","name":"mysql::excute","stp":"2101"},{"E":4,"EXP":"test this exception","S":13,"clues":["40:/support/c-cpp-php-python","46:300"],"dst":"www.pinpoint-wonderful.com","name":"httpclient","nsid":"87969596","stp":"9800"}],"client":"127.0.0.1","clues":["46:200"],"name":"C_CPP Request","server":"HTTP_HOST","sid":"53562116","stp":"1500","tid":"c_test_app^1614586481657^5801","uri":"test_url"}`

		// msg := `{"E":1,"FT":1500,"NP":"t=1617083759.535 D=0.000","S":1617083759798,"appid":"app-2","appname":"APP-2","calls":[{"E":1,"S":0,"clues":["-1:input parameters","14:return value"],"name":"app\\AppDate::outputDate","stp":"1501"}],"client":"10.34.135.145","clues":["46:200"],"name":"PHP Request: fpm-fcgi","server":"10.34.130.152:8000","sid":"726125302","stp":"1500","tid":"app-2^1617083747^5506","uri":"/index.php?type=get_date"}`
		buffer := generateValidPacket(msg)
		_, e := conn.Write(buffer)
		if e != nil {
			t.Error("Error to send message because of ", e.Error())
			break
		}
		time.Sleep(100 * time.Millisecond)
	}
	conn.Write(genUniqueIdBody())
	readId(t, conn)

	conn.Write(generateInvalidPacket03())
	conn.Write(generateInvalidPacket02())
}

func readHello(t *testing.T, conn net.Conn) {
	time.Sleep(5 * time.Second)
	buf := make([]byte, 1280)
	conn.Read(buf)
	// if err != nil {
	// 	t.Error("Error to read message because of ", err)
	// 	return
	// }
	hello := string(buf[8:])
	if !strings.HasPrefix(hello, "{\"appid\":\"") {
		t.Error(hello)
	}
}

func readId(t *testing.T, conn net.Conn) {
	time.Sleep(5 * time.Second)
	buf := make([]byte, 1280)
	conn.Read(buf)
	id := string(buf[8:])
	if !strings.HasPrefix(id, "{\"uid\"") {
		t.Error(id)
	}
}

func TestJsonServer(t *testing.T) {
	js := launchJsonServer()
	go js.Run()
	time.Sleep(2 * time.Second)
	conn, err := net.Dial("tcp", "127.0.0.1:8789")
	if err != nil {
		t.Error(err)
	}
	readHello(t, conn)
	handleWrite(t, conn)
}

func TestMain(m *testing.M) {
	log.SetLevel(log.DebugLevel)
	log.SetOutput(os.Stdout)
	code := m.Run()

	os.Exit(code)
}

func TestLoadConfig(t *testing.T) {
	os.Setenv("PP_COLLECTOR_AGENT_SPAN_IP", "dev-pinpoint")
	os.Setenv("PP_COLLECTOR_AGENT_SPAN_PORT", "9993")
	os.Setenv("PP_COLLECTOR_AGENT_AGENT_IP", "dev-pinpoint")
	os.Setenv("PP_COLLECTOR_AGENT_AGENT_PORT", "9991")
	os.Setenv("PP_COLLECTOR_AGENT_STAT_IP", "dev-pinpoint")
	os.Setenv("PP_COLLECTOR_AGENT_STAT_PORT", "9992")
	os.Setenv("PP_COLLECTOR_AGENT_ISDOCKER", "true")
	os.Setenv("PP_LOG_DIR", "/tmp/")
	os.Setenv("PP_Log_Level", "ERROR")
	os.Setenv("PP_ADDRESS", "0.0.0.0@9999")
	InitServerConfig()
}
