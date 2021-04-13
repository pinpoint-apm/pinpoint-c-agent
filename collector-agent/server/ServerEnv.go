package server

import (
	"CollectorAgent/common"
	"fmt"
	"os"
	"path"
	"strconv"
	"strings"

	log "github.com/sirupsen/logrus"
	prefixed "github.com/x-cray/logrus-prefixed-formatter"
	"gopkg.in/natefinch/lumberjack.v2"
)

/**
# pinpoint collector span ip
PP_COLLECTOR_AGENT_SPAN_IP=dev-pinpoint
# pinpoint collector span port
PP_COLLECTOR_AGENT_SPAN_PORT=9993
# pinpoint collector agent IP
PP_COLLECTOR_AGENT_AGENT_IP=dev-pinpoint
# pinpoint collector agent port
PP_COLLECTOR_AGENT_AGENT_PORT=9991
# pinpoint collector stat ip
PP_COLLECTOR_AGENT_STAT_IP=dev-pinpoint
# pinpoint collector stat port
PP_COLLECTOR_AGENT_STAT_PORT=9992
# mark all agent a docker
PP_COLLECTOR_AGENT_ISDOCKER=true
PP_LOG_DIR=/tmp/
PP_Log_Level=ERROR
# binds on
PP_ADDRESS=0.0.0.0@9999
*/

func InitServerConfig() {
	config := common.GetConfig()

	spanIp := os.Getenv("PP_COLLECTOR_AGENT_SPAN_IP")
	spanPort := os.Getenv("PP_COLLECTOR_AGENT_SPAN_PORT")
	config.SpanAddress = spanIp + ":" + spanPort
	if config.SpanAddress == ":" {
		panic("Invalid SpanAddress")
	}

	statIp := os.Getenv("PP_COLLECTOR_AGENT_STAT_IP")
	statPort := os.Getenv("PP_COLLECTOR_AGENT_STAT_PORT")
	config.StatAddress = statIp + ":" + statPort

	if config.StatAddress == ":" {
		panic("Invalid StatAddress")
	}

	agentIp := os.Getenv("PP_COLLECTOR_AGENT_AGENT_IP")
	agentPort := os.Getenv("PP_COLLECTOR_AGENT_AGENT_PORT")
	config.AgentAddress = agentIp + ":" + agentPort
	if config.AgentAddress == ":" {
		panic("Invalid AgentAddress")
	}

	inDocker := os.Getenv("PP_COLLECTOR_AGENT_ISDOCKER")
	if b, err := strconv.ParseBool(inDocker); err == nil {
		config.Container = b
	} else {
		config.Container = false
	}

	logLevel := os.Getenv("PP_Log_Level")
	config.LoggerLevel = logLevel
	logDir := os.Getenv("PP_LOG_DIR")
	config.LoggerDir = logDir
	address := os.Getenv("PP_ADDRESS")
	if len(address) == 0 {
		panic("PP_ADDRESS invalid")
	}
	config.Address = strings.Replace(address, "@", ":", 1)
	config.SocketType = "tcp"
	// initialize logger
	if level, err := log.ParseLevel(config.LoggerLevel); err == nil {
		log.SetLevel(level)
	}

	if _, err := os.Stat(config.LoggerDir); os.IsNotExist(err) {
		msg := fmt.Sprintf("PP_LOG_DIR: %s not exist", config.LoggerDir)
		panic(msg)
	}

	logFile := path.Join(config.LoggerDir, "collector.log")
	// bind logger on the file
	logger := &lumberjack.Logger{
		Filename:   logFile,
		MaxSize:    500,
		MaxBackups: 500,
	}

	log.SetOutput(logger)
	log.SetFormatter(&prefixed.TextFormatter{
		DisableColors:   true,
		TimestampFormat: "2006-01-02 15:04:05.999999999",
		FullTimestamp:   true,
		ForceFormatting: true,
	})

	log.Info(config)
}
