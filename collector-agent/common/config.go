package common

import (
	"fmt"
	"net"
	"os"
	"time"

	v1 "github.com/pinpoint-apm/pinpoint-c-agent/collector-agent/pinpoint-grpc-idl-go/proto/v1"
	"google.golang.org/grpc"
)

type Config struct {
	SocketType   string
	Address      string
	StatAddress  string
	SpanAddress  string
	AgentAddress string

	WebPorts    int16
	LoggerLevel string
	LoggerDir   string
	// tuning
	AgentChannelSize          uint64
	SpanStreamParallelismSize int32

	AgentReTryTimeout     time.Duration
	PingInterval          time.Duration
	StatInterval          time.Duration
	SpanTimeWaitSec       time.Duration
	MetaDataTimeWaitSec   time.Duration
	GrpcConTextTimeOutSec time.Duration
	// agent auto offline after xxxx
	AgentFreeOnlineSurvivalTimeSec time.Duration
	// agentInfo
	HostName   string
	HostIp     string
	Pid        int32
	StartTime  int64
	ServerType int32
	Container  bool
}

var config = &Config{
	WebPorts:                       80,
	AgentChannelSize:               1000,
	SpanStreamParallelismSize:      2,
	AgentReTryTimeout:              10,
	PingInterval:                   300,
	StatInterval:                   5,
	SpanTimeWaitSec:                10,
	MetaDataTimeWaitSec:            10,
	GrpcConTextTimeOutSec:          5,
	AgentFreeOnlineSurvivalTimeSec: 1 * 60 * 60, // 1 hour
	ServerType:                     1700,        // PHP
	LoggerLevel:                    "DEBUG",
	LoggerDir:                      "/tmp/",
	StartTime:                      time.Now().Unix(),
	Pid:                            int32(os.Getgid()),
	HostName:                       getHostName(),
	HostIp:                         lookupIpFromName(getHostName()),
}

func getHostName() string {
	name, err := os.Hostname()
	if err != nil {
		return "localhost"
	}
	return name
}

func lookupIpFromName(name string) string {
	conn, err := net.Dial("udp", "8.8.8.8:80")
	if err == nil {
		defer conn.Close()
		ipAddress := conn.LocalAddr().(*net.UDPAddr)
		return ipAddress.IP.String()
	}
	return "127.0.0.1"
}

//var once sync.Once

func GetConfig() *Config {
	//once.Do(
	//	func() {
	//		//tuning
	//		config = &Config{
	//			WebPorts:                  80,
	//			AgentChannelSize:          1000,
	//			SpanStreamParallelismSize: 2,
	//			AgentReTryTimeout:         10,
	//			PingInterval:              300,
	//			StatInterval:              300,
	//			SpanTimeWaitSec:           10,
	//			MetaDataTimeWaitSec:       10,
	//
	//			ServerType:  1700, // PHP
	//			LoggerLevel: "DEBUG",
	//			LoggerDir:   "/tmp/",
	//			StartTime:   time.Now().Unix(),
	//			Pid:         int32(os.Getgid()),
	//		}
	//	})
	return config
}

/**
WithInsecure,WithBlock
*/
func GetDialOption() []grpc.DialOption {
	var opts []grpc.DialOption
	opts = append(opts, grpc.WithInsecure())
	opts = append(opts, grpc.WithBlock())
	return opts
}

func GetPBAgentInfo(serverType int32) *v1.PAgentInfo {
	agentInfo := &v1.PAgentInfo{
		Hostname:     config.HostName,
		Ip:           config.HostIp,
		Pid:          config.Pid,
		ServiceType:  serverType,
		Container:    config.Container,
		EndTimestamp: -1,
	}

	return agentInfo
}

func (self *Config) String() string {
	return fmt.Sprintf("SocketType:%s Address:%s  StatAddress:%s SpanAddress:%s AgentAddress%s WebPorts:%d  LoggerLevel:%s LoggerDir:%s AgentChannelSize:%d SpanStreamParallelismSize:%d AgentReTryTimeout:%s PingInterval:%d StatInterval:%d SpanTimeWaitSec:%d MetaDataTimeWaitSec:%d Pid:%d StartTime:%d ServerType:%d Container:%t", self.SocketType, self.Address, self.StatAddress, self.SpanAddress, self.AgentAddress, self.WebPorts, self.LoggerLevel, self.LoggerDir, self.AgentChannelSize, self.SpanStreamParallelismSize, self.AgentReTryTimeout, self.PingInterval, self.StatInterval, self.SpanTimeWaitSec, self.MetaDataTimeWaitSec, self.Pid, self.StartTime, self.ServerType, self.Container)
}
