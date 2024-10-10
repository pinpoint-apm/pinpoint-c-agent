package common

import (
	"context"
	"fmt"
	"io"
	"net"
	"os"
	"path"
	"strings"
	"time"

	v1 "github.com/pinpoint-apm/pinpoint-c-agent/collector-agent/pinpoint-grpc-idl-go/proto/v1"
	"github.com/sirupsen/logrus"
	prefixed "github.com/x-cray/logrus-prefixed-formatter"
	"google.golang.org/grpc"
	"google.golang.org/grpc/credentials/insecure"
	"gopkg.in/natefinch/lumberjack.v2"
)

type UserSetting struct {
	RecvBufSize  int // default is 4k
	BindAddress  string
	SpanAddress  string
	AgentAddress string
	StatAddress  string
	Container    bool
	LoggerLevel  string
	LoggerDir    string
	LogStdout    bool
	Profile      bool
}

func (u *UserSetting) String() string {
	return fmt.Sprintf("{RecvBufSize:%v BindAddress:%v SpanAddress:%v AgentAddress:%v StatAddress:%v}", u.RecvBufSize, u.BindAddress, u.SpanAddress, u.AgentAddress, u.StatAddress)
}

type Config struct {
	Log        *logrus.Logger
	LogEntry   *logrus.Entry
	GrpcOption []grpc.DialOption

	AgentChannelSize          uint64
	SpanStreamParallelismSize int32

	AgentReTryTimeout  time.Duration
	PingInterval       time.Duration
	StatInterval       time.Duration
	SpanTimeWait       time.Duration
	MetaDataTimeWait   time.Duration
	GrpcConTextTimeOut time.Duration
	AgentRetireTime    time.Duration
	HostName           string
	HostIp             string
	Pid                int32
	StartTime          int64
	User               *UserSetting
}

func (c *Config) ParseServerAddress() (socket_type string, address string) {
	raw_address := c.User.BindAddress

	if strings.HasPrefix(raw_address, "sock") {
		// /tmp/pinpoint.sock
		// a very loose checking
		// assume a file
		return "unix", address
	} else {
		// like 0.0.0.0:5689
		return "tcp", strings.Replace(raw_address, "@", ":", 1)
	}
}

func getHostName() string {
	name, err := os.Hostname()
	if err != nil {
		return "localhost"
	}
	return name
}

func lookupIpFromName() string {
	conn, err := net.Dial("udp", "8.8.8.8:80")
	if err == nil {
		defer conn.Close()
		ipAddress := conn.LocalAddr().(*net.UDPAddr)
		return ipAddress.IP.String()
	}
	return "127.0.0.1"
}

func CreateDefaultConfig() *Config {
	config := &Config{
		AgentChannelSize:          1000,
		SpanStreamParallelismSize: 2,
		AgentReTryTimeout:         10 * time.Second,
		PingInterval:              5 * time.Minute,
		StatInterval:              5 * time.Second,
		SpanTimeWait:              10 * time.Second,
		MetaDataTimeWait:          10 * time.Second,
		GrpcConTextTimeOut:        5 * time.Second,
		AgentRetireTime:           1 * time.Hour,
		StartTime:                 time.Now().Unix(),
		Pid:                       int32(os.Getgid()),
		HostName:                  getHostName(),
		HostIp:                    lookupIpFromName(),
		Log:                       logrus.New(),
	}
	config.GrpcOption = append(config.GrpcOption,
		grpc.WithTransportCredentials(insecure.NewCredentials()),
		grpc.WithBlock())
	config.LogEntry = config.Log.WithField("id", "main")
	return config
}

func CreateTestConfig() *Config {
	config := CreateDefaultConfig()
	user := &UserSetting{
		AgentAddress: "dev-pinpoint:9991",
		SpanAddress:  "dev-pinpoint:9993",
		StatAddress:  "dev-pinpoint:9992",
		LoggerLevel:  "debug",
		LogStdout:    true,
		RecvBufSize:  4 * 1024,
	}
	config.User = user
	config.InitLogger()
	return config
}

func (config *Config) CreateGrpcConnection(parent_ctx context.Context, address string) (*grpc.ClientConn, error) {
	ctx, cancel := context.WithTimeout(parent_ctx, config.GrpcConTextTimeOut)
	defer cancel()
	conn, err := grpc.DialContext(ctx, address, config.GrpcOption...)
	if err != nil {
		return nil, err
	}
	return conn, nil
}

func (config *Config) InitLogger() {
	if level, err := logrus.ParseLevel(config.User.LoggerLevel); err == nil {
		config.Log.SetLevel(level)
	}

	var log_writers []io.Writer

	if _, err := os.Stat(config.User.LoggerDir); err == nil {

		logFile := path.Join(config.User.LoggerDir, "collector.log")
		// bind logger on the file
		logger := &lumberjack.Logger{
			Filename:   logFile,
			MaxSize:    50,
			MaxBackups: 50,
		}

		log_writers = append(log_writers, logger)
	}

	if config.User.LogStdout {
		log_writers = append(log_writers, os.Stdout)
	}

	config.Log.SetOutput(io.MultiWriter(log_writers...))

	config.Log.SetFormatter(&prefixed.TextFormatter{
		ForceColors:     true,
		TimestampFormat: "2006-01-02 15:04:05.999",
		FullTimestamp:   true,
		ForceFormatting: true,
	})
}

func GetPBAgentInfo(serverType int32, config *Config) *v1.PAgentInfo {
	agentInfo := &v1.PAgentInfo{
		Hostname:     config.HostName,
		Ip:           config.HostIp,
		Pid:          config.Pid,
		ServiceType:  serverType,
		Container:    config.User.Container,
		EndTimestamp: -1,
	}

	return agentInfo
}
