package main

import (
	"flag"
	"fmt"
	"net/http"
	_ "net/http/pprof"
	"os"
	"runtime"
	"strconv"

	"github.com/pinpoint-apm/pinpoint-c-agent/collector-agent/common"
	"github.com/pinpoint-apm/pinpoint-c-agent/collector-agent/server"
	log "github.com/sirupsen/logrus"
)

var (
	agent_address   = flag.String("AgentAddress", "localhost:9991", "Set recv buf; eg: -AgentAddress=localhost:9991")
	span_address    = flag.String("SpanAddress", "localhost:9991", "Set recv buf; eg: -SpanAddress=localhost:9993")
	stat_address    = flag.String("StatAddress", "localhost:9991", "Set recv buf; eg: -StatAddress=localhost:9992")
	bind_address    = flag.String("host", "", "server bind host and port information; eg: -host=0.0.0.0@10000")
	in_container    = flag.Bool("container", false, "collector-agent run in a pod or PM; eg: -container=true")
	log_dir         = flag.String("LogDir", os.TempDir(), "Set logging output directory; eg: -LogDir=/tmp")
	log_stdout      = flag.Bool("LogStdout", true, "enable net/http/pprof")
	log_level       = flag.String("LogLevel", "debug", "Set logging output level(debug/info/warn/error); eg: -LogLevel=info")
	server_recv_buf = flag.Int("RecvBufSize", 4096*100, "Set recv buf; eg: -RecvBufSize=409600")
	enable_profile  = flag.Bool("EnableProfile", false, "enable net/http/pprof")
	show_version    = flag.Bool("v", false, "show current version and exit")
)

func parseConfig() *common.Config {
	setting := &common.UserSetting{
		RecvBufSize:  *server_recv_buf,
		BindAddress:  *bind_address,
		SpanAddress:  *span_address,
		AgentAddress: *agent_address,
		StatAddress:  *stat_address,
		Container:    *in_container,
		LoggerLevel:  *log_level,
		LogStdout:    *log_stdout,
		LoggerDir:    *log_dir,
	}
	if ip, ok := os.LookupEnv("PP_COLLECTOR_AGENT_SPAN_IP"); ok {
		if port, ok := os.LookupEnv("PP_COLLECTOR_AGENT_SPAN_PORT"); ok {
			setting.SpanAddress = fmt.Sprintf("%s:%s", ip, port)
		}
	}

	if ip, ok := os.LookupEnv("PP_COLLECTOR_AGENT_STAT_IP"); ok {
		if port, ok := os.LookupEnv("PP_COLLECTOR_AGENT_STAT_PORT"); ok {
			setting.StatAddress = fmt.Sprintf("%s:%s", ip, port)
		}
	}

	if ip, ok := os.LookupEnv("PP_COLLECTOR_AGENT_AGENT_IP"); ok {
		if port, ok := os.LookupEnv("PP_COLLECTOR_AGENT_AGENT_PORT"); ok {
			setting.AgentAddress = fmt.Sprintf("%s:%s", ip, port)
		}
	}

	if in, ok := os.LookupEnv("PP_COLLECTOR_AGENT_ISDOCKER"); ok {
		if b, err := strconv.ParseBool(in); err == nil {
			setting.Container = b
		}
	}

	if v, ok := os.LookupEnv("PP_Log_Level"); ok {
		setting.LoggerLevel = v
	}

	if v, ok := os.LookupEnv("PP_LOG_DIR"); ok {
		setting.LoggerDir = v
	}

	if v, ok := os.LookupEnv("PP_ADDRESS"); ok {
		setting.BindAddress = v
	}

	config := common.CreateDefaultConfig()
	config.User = setting
	config.InitLogger()
	return config
}

func main() {
	flag.Parse()

	config := parseConfig()

	if *show_version {
		fmt.Fprintf(os.Stderr, "collector-agent:%s \r\n", server.Version)
		return
	}

	if *enable_profile {
		go func() {
			log.Println(http.ListenAndServe("0.0.0.0:8081", nil))
			runtime.SetBlockProfileRate(1)
		}()
	}

	if config.User.BindAddress == "" {
		flag.Usage()
		return
	}

	config.Log.Infof("Config:{%v}", config)
	server := server.CreateServer(parseConfig())

	if _, err := server.Run(); err != nil {
		config.Log.Warn("SpanServer is exit")
	}
}
