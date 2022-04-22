package agent

import (
	"context"
	"errors"
	"fmt"
	"strconv"
	"sync"
	"time"

	"github.com/pinpoint-apm/pinpoint-c-agent/collector-agent/common"
	v1 "github.com/pinpoint-apm/pinpoint-c-agent/collector-agent/protocol"

	log "github.com/sirupsen/logrus"
	"google.golang.org/grpc"
	"google.golang.org/grpc/metadata"
	"google.golang.org/protobuf/types/known/wrapperspb"
)

type GrpcAgent struct {
	AgentId        string
	agentName      string
	agentType      int32
	StartTime      string
	BaseMD         metadata.MD
	pingMd         metadata.MD
	PingId         int32
	spanFilter     []Filter
	spanSender     SpanSender
	AgentOnLine    bool
	requestCounter RequestProfiler
	tasksGroup     sync.WaitGroup
	jsonSpan       chan map[string]interface{}
	ExitCh         chan bool
	log            *log.Entry
}

func (agent *GrpcAgent) SendSpan(span map[string]interface{}) {
	defer func() {
		if r := recover(); r != nil {
			log.Warnf("sendSpan met:%s", r)
		}
	}()

	agent.jsonSpan <- span
}

func (agent *GrpcAgent) GetLastBusyTime() int64 {
	return agent.requestCounter.CTime
}

func (agent *GrpcAgent) Stop() {
	agent.log.Warn("I'm exiting")
	close(agent.jsonSpan)
	close(agent.ExitCh)
	agent.tasksGroup.Wait()
	agent.log.Warn("I'm exit")
}

func (agent *GrpcAgent) AddFilter(filter Filter) {
	agent.spanFilter = append(agent.spanFilter, filter)
}

func (agent *GrpcAgent) Interceptor(map[string]interface{}) bool {
	return agent.AgentOnLine
}

func (agent *GrpcAgent) String() string {
	return fmt.Sprintf("id:%s name:%s type:%d startTime:%s", agent.AgentId, agent.agentName, agent.agentType, agent.StartTime)
}

func (agent *GrpcAgent) agentOnline() error {

	commandTask := sync.WaitGroup{}
	defer commandTask.Wait()

	config := common.GetConfig()
	agent.log.Debugf("connect AgentChannel:%s for agentOnline", config.AgentAddress)

	conn, err := common.CreateGrpcConnection(config.AgentAddress)
	if err != nil {
		agent.log.Warnf("connect %s timeout", config.AgentAddress)
		return errors.New("connect pinpoint-collector timeout")
	}

	defer func() {
		agent.log.Info("client activity close grpc connection")
		if err := conn.Close(); err != nil {
			agent.log.Warnf("close connection:%s", err)
		}
	}()

	client := v1.NewAgentClient(conn)
	ctx, cancel := common.BuildPinpointCtx(config.GrpcConTextTimeOutSec, agent.BaseMD)
	defer cancel()
	pbAgentInfo := common.GetPBAgentInfo(agent.agentType)
	agent.log.Debugf("RequestAgentInfo pbAgentInfo:%v", pbAgentInfo)
	if res, err := client.RequestAgentInfo(ctx, &pbAgentInfo); err != nil {
		errorMsg := fmt.Sprintf("RequestAgentInfo failed. %s", err)
		agent.log.Warn(errorMsg)
		return errors.New(errorMsg)
	} else {
		agent.log.Debugf("RequestAgentInfo response:%s", res)
	}

	// send ping
	pingCtx := metadata.NewOutgoingContext(context.Background(), agent.pingMd)

	stream, err := client.PingSession(pingCtx)
	if err != nil {
		errorMsg := fmt.Sprintf("Get PingSession Failed:%s", err)
		agent.log.Error(errorMsg)
		return errors.New(errorMsg)
	}

	defer func() {
		if err := stream.CloseSend(); err != nil {
			agent.log.Warnf("unwanted err when stream.CloseSend:%s", err)
		}
	}()

	// handle command
	go agent.handleCommand(conn, &commandTask)

	agent.AgentOnLine = true

	defer func() { agent.AgentOnLine = false }()

	ping := v1.PPing{}
	for {
		// send ping
		agent.log.Infof("ping  %s %v", agent, agent.pingMd)
		if err := stream.Send(&ping); err != nil {
			agent.log.Warnf("agentOnline Send  ping failed. %s", err)
			return err
		}
		// recv ping
		if _, err := stream.Recv(); err != nil {
			agent.log.Warnf("agentOnline recv ping failed. %s", err)
			return err
		}

		if common.WaitChannelEvent(agent.ExitCh, config.PingInterval) == common.E_AGENT_STOPPING {
			return errors.New("catch exit during ping")
		}
	}
}

func (agent *GrpcAgent) keepAgentOnline() {
	agent.tasksGroup.Add(1)
	defer agent.tasksGroup.Done()

	for {
		if err := agent.agentOnline(); err != nil {
			agent.log.Infof("agent online exit:%s ", err)
		}

		config := common.GetConfig()
		if common.WaitChannelEvent(agent.ExitCh, config.PingInterval) == common.E_AGENT_STOPPING {
			break
		}
	}
	agent.spanSender.Stop()
}

func (agent *GrpcAgent) registerFilter() {
	// online/off
	agent.log.Debug("register agent filter")
	agent.AddFilter(agent)
	// req count
	agent.log.Debug("register requestCounter filter")
	agent.AddFilter(&agent.requestCounter)

	// send span
	agent.log.Debug("register spanSender filter")
	agent.AddFilter(&agent.spanSender)

}

func (agent *GrpcAgent) sendStat() {

	config := common.GetConfig()
	agent.log.Debugf("connect StatAddress:%s", config.StatAddress)

	conn, err := common.CreateGrpcConnection(config.StatAddress)
	if err != nil {
		errorMsg := fmt.Sprintf("Dail %s failed err:%s", config.StatAddress, err)
		agent.log.Warn(errorMsg)
		return
	}

	defer func() {
		if err := conn.Close(); err != nil {
			agent.log.Warnf("conn close with:%s", err)
		}
	}()

	ctx, _ := common.BuildPinpointCtx(-1, agent.pingMd)

	client := v1.NewStatClient(conn)

	stream, err := client.SendAgentStat(ctx)
	if err != nil {
		errorMsg := fmt.Sprintf("create stat client failed:%s", config.StatAddress)
		agent.log.Warn(errorMsg)
		return
	}

	for {
		msg := CollectPStateMessage(agent.requestCounter.GetMaxAvg, agent.requestCounter.GetReqTimeProfiler)

		agent.log.Infof("%v", msg)
		if err := stream.Send(msg); err != nil {
			agent.log.Warn(err)
			return
		}
		//config.StatInterval
		if common.WaitChannelEvent(agent.ExitCh, 0) == common.E_AGENT_STOPPING {
			return
		}
	}

}

func (agent *GrpcAgent) uploadStatInfo() {
	agent.tasksGroup.Add(1)
	defer agent.tasksGroup.Done()

	for {
		agent.sendStat()

		config := common.GetConfig()
		if common.WaitChannelEvent(agent.ExitCh, config.StatInterval) == common.E_AGENT_STOPPING {
			return
		}
	}
}

func (agent *GrpcAgent) Init(id, _name string, _type int32, StartTime string) {
	agent.log = log.WithFields(log.Fields{"appid": id})

	agent.AgentId = id
	agent.agentName = _name
	agent.agentType = _type
	agent.StartTime = StartTime
	agent.BaseMD = metadata.New(map[string]string{
		"starttime":       StartTime,
		"agentid":         id,
		"applicationname": _name,
	})

	pingIdStr := strconv.FormatInt(int64(agent.PingId), 10)

	agent.pingMd = metadata.New(map[string]string{
		"starttime":       agent.StartTime,
		"agentid":         agent.AgentId,
		"applicationname": agent.agentName,
		"socketid":        pingIdStr,
	})

	config := common.GetConfig()

	agent.jsonSpan = make(chan map[string]interface{}, config.AgentChannelSize)
	agent.ExitCh = make(chan bool)
	agent.spanSender = SpanSender{Md: agent.BaseMD, ExitCh: agent.ExitCh}
	agent.spanSender.Init()
	agent.requestCounter.CTime = time.Now().Unix()
	agent.registerFilter()
	// start agentOnline
	go agent.keepAgentOnline()
	// send stat
	go agent.uploadStatInfo()

}

func (agent *GrpcAgent) Start() {
	go agent.consumeJsonSpan()
}

func (agent *GrpcAgent) collectorActiveThreadCount(conn *grpc.ClientConn, responseId int32, interval time.Duration, wg *sync.WaitGroup) {
	wg.Add(1)
	defer wg.Done()

	client := v1.NewProfilerCommandServiceClient(conn)
	ctx, _ := common.BuildPinpointCtx(-1, agent.pingMd)

	if activeThreadCountClient, err := client.CommandStreamActiveThreadCount(ctx); err == nil {
		sequenceId := int32(1)
		for {
			agent.log.Infof("ResponseId %d", responseId)
			response := v1.PCmdStreamResponse{
				ResponseId: responseId,
				SequenceId: sequenceId,
				Message: &wrapperspb.StringValue{
					Value: "hello",
				},
			}
			sequenceId += 1

			res := v1.PCmdActiveThreadCountRes{
				CommonStreamResponse: &response,
			}

			for _, value := range agent.requestCounter.GetReqTimeProfiler() {
				res.ActiveThreadCount = append(res.ActiveThreadCount, int32(value))
			}

			res.TimeStamp = time.Now().Unix()
			res.HistogramSchemaType = 2

			agent.log.Infof("try to send PCmdActiveThreadCountRes:%v", res)

			if err := activeThreadCountClient.Send(&res); err != nil {
				agent.log.Infof("collectorActiveThreadCount:responseId:%d end with:%s", responseId, err)
				break
			}

			if common.WaitChannelEvent(agent.ExitCh, interval) == common.E_AGENT_STOPPING {
				agent.log.Info("catch exit during send collectorActiveThreadCount")
				break
			}
		}

	}
}

func (agent *GrpcAgent) genCmdHandshake() *v1.PCmdMessage {
	handshake := v1.PCmdServiceHandshake{}
	handshake.SupportCommandServiceKey = append(handshake.SupportCommandServiceKey, int32(v1.PCommandType_PING))
	handshake.SupportCommandServiceKey = append(handshake.SupportCommandServiceKey, int32(v1.PCommandType_PONG))
	handshake.SupportCommandServiceKey = append(handshake.SupportCommandServiceKey, int32(v1.PCommandType_ECHO))
	handshake.SupportCommandServiceKey = append(handshake.SupportCommandServiceKey, int32(v1.PCommandType_ACTIVE_THREAD_COUNT))
	handshake.SupportCommandServiceKey = append(handshake.SupportCommandServiceKey, int32(v1.PCommandType_ACTIVE_THREAD_DUMP))
	handshake.SupportCommandServiceKey = append(handshake.SupportCommandServiceKey, int32(v1.PCommandType_ACTIVE_THREAD_LIGHT_DUMP))

	return &v1.PCmdMessage{
		Message: &v1.PCmdMessage_HandshakeMessage{
			HandshakeMessage: &handshake,
		},
	}
}

func (agent *GrpcAgent) handleCommand(conn *grpc.ClientConn, wg *sync.WaitGroup) {
	defer wg.Done()
	wg.Add(1)

	cmdWg := sync.WaitGroup{}
	defer cmdWg.Wait()

	//
	//config := common.GetConfig()
	//agent.log.Debugf("connect AgentChannel:%s for agentOnline", config.AgentAddress)
	//conn, err := grpc.Dial(config.AgentAddress, common.GetDialOption()...)
	//if err != nil {
	//	errorMsg := fmt.Sprintf("Dail %s failed", config.AgentAddress)
	//	agent.log.Warn(errorMsg)
	//}

	client := v1.NewProfilerCommandServiceClient(conn)
	//config.AgentReTryTimeout
	ctx, _ := common.BuildPinpointCtx(-1, agent.pingMd)

	commandClient, err := client.HandleCommand(ctx)

	if err != nil {
		agent.log.Warnf("handleCommand got err:%s", err)
		return
	}

	// send handleshake
	if err := commandClient.Send(agent.genCmdHandshake()); err != nil {
		agent.log.Warnf("handleCommand Send got err:%s", err)
		return
	}
	agent.log.Debugf("send command handshake %s", agent.genCmdHandshake())
	for {
		if cmd, err := commandClient.Recv(); err != nil {
			agent.log.Warnf("handleCommand.Recv got err:%s", err)
			return
		} else {
			agent.log.Infof("appid:%s handleCommand: get cmd %s", agent.AgentId, cmd)
			switch cmd.Command.(type) {
			case *v1.PCmdRequest_CommandEcho:
				{
					agent.log.Debug("PCmdRequest_CommandEcho")
				}
			case *v1.PCmdRequest_CommandActiveThreadCount:
				// create a new coro to send active thread
				go agent.collectorActiveThreadCount(conn, cmd.RequestId, 1, &cmdWg)
			case *v1.PCmdRequest_CommandActiveThreadDump:
				agent.log.Debug("PCmdRequest_CommandActiveThreadDump")
			case *v1.PCmdRequest_CommandActiveThreadLightDump:
				response := v1.PCmdResponse{
					ResponseId: cmd.RequestId,
				}

				dumpRes := v1.PCmdActiveThreadLightDumpRes{
					Type:           "java",
					SubType:        "oracle",
					Version:        "1.8.105",
					CommonResponse: &response,
				}

				go func(in *v1.PCmdActiveThreadLightDumpRes) {
					defer cmdWg.Done()
					cmdWg.Add(1)

					if _, err := client.CommandActiveThreadLightDump(ctx, in); err != nil {
						agent.log.Warnf("CommandActiveThreadLightDump failed! err:%s", err)
					}
				}(&dumpRes)

			default:
				agent.log.Warnf("unknown command type %v", cmd)
			}
		}
	}

}

func (agent *GrpcAgent) consumeJsonSpan() {
	defer agent.tasksGroup.Done()
	agent.tasksGroup.Add(1)

	for span := range agent.jsonSpan {
		if span == nil {
			agent.log.Infof("agent:%v get EOF", agent)
			return
		}
		for i := range agent.spanFilter {
			if agent.spanFilter[i].Interceptor(span) == false {
				break
			}
		}
	}
}

func (agent *GrpcAgent) CheckValid(name string, ft int32) bool {
	if name != agent.agentName || ft != agent.agentType {
		agent.log.Warn("name or FT not equal")
		return false
	} else {
		return true
	}

}
