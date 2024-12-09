package agent

import (
	"context"
	"errors"
	"fmt"
	"strconv"
	"sync"
	"time"

	"github.com/pinpoint-apm/pinpoint-c-agent/collector-agent/common"
	v1 "github.com/pinpoint-apm/pinpoint-c-agent/collector-agent/pinpoint-grpc-idl-go/proto/v1"
	"github.com/shirou/gopsutil/cpu"
	"github.com/shirou/gopsutil/v3/mem"

	"github.com/sirupsen/logrus"
	"google.golang.org/grpc"
	"google.golang.org/grpc/metadata"
	"google.golang.org/protobuf/types/known/wrapperspb"
)

type GrpcAgent struct {
	config      *common.Config
	AgentId     string
	agentName   string
	agentType   int32
	StartTime   string
	metadata    metadata.MD
	pingMd      metadata.MD
	PingId      int32
	spanFilters []Filter
	AgentOnLine bool
	reqCounter  *RequestCounter
	utReport    *UrlTemplateReport
	tasksGroup  sync.WaitGroup
	tSpanBufCh  chan *TSpan
	ctx         context.Context
	cancel_ctx  context.CancelFunc
	log         *logrus.Entry
}

func CreateGrpcAgent(id, name string, agentType, pingId int32, startTime string, config *common.Config) *GrpcAgent {
	agent := &GrpcAgent{
		PingId:      pingId,
		AgentOnLine: false,
		config:      config,
		log:         config.Log.WithField("app-id", id),
		AgentId:     id,
		agentName:   name,
		agentType:   agentType,
		StartTime:   startTime,
		metadata: metadata.New(map[string]string{
			"starttime":       startTime,
			"agentid":         id,
			"applicationname": name,
		}),
		pingMd: metadata.New(map[string]string{
			"starttime":       startTime,
			"agentid":         id,
			"applicationname": name,
			"socketid":        strconv.FormatInt(int64(pingId), 10),
		}),
		utReport:   CreateUrlTemplateReport(),
		tSpanBufCh: make(chan *TSpan, config.AgentChannelSize),
	}

	agent.ctx, agent.cancel_ctx = context.WithCancel(context.Background())
	agent.reqCounter = createRequestCounter(config)
	agent.runBackgroundTasks()
	config.Log.Infof("agent:%v is launched", agent)

	return agent
}

func (agent *GrpcAgent) SendSpan(span *TSpan) {
	defer func() {
		if r := recover(); r != nil {
			agent.log.Warnf("sendSpan met:%s", r)
		}
	}()
	agent.tSpanBufCh <- span
}

func (agent *GrpcAgent) GetLastBusyTime() int64 {
	return agent.reqCounter.CTime
}

func (agent *GrpcAgent) Stop() {
	agent.log.Infof("I'm exiting")
	agent.cancel_ctx()
	for _, filter := range agent.spanFilters {
		filter.Stop()
	}

	agent.tasksGroup.Wait()
	agent.log.Warn("I'm exit")
}

func (agent *GrpcAgent) AddFilter(filter Filter) {
	agent.spanFilters = append(agent.spanFilters, filter)
}

// func (agent *GrpcAgent) Interceptor(_ *TSpan) bool {
// 	if !agent.AgentOnLine {
// 		agent.log.Debugf("agent offline")
// 	}

// 	return true
// }

func (agent *GrpcAgent) String() string {
	return fmt.Sprintf("id:%s name:%s type:%d startTime:%s", agent.AgentId, agent.agentName, agent.agentType, agent.StartTime)
}

func (a *GrpcAgent) createMdCtxWithTime(dur time.Duration, md metadata.MD) (ctx context.Context, cancel context.CancelFunc) {
	ctx, cancel = context.WithTimeout(a.ctx, dur)
	ctx = metadata.NewOutgoingContext(ctx, md)
	return ctx, cancel
}

func (a *GrpcAgent) createMdCtx(md metadata.MD) (ctx context.Context, cancel context.CancelFunc) {
	ctx, cancel = context.WithCancel(a.ctx)
	ctx = metadata.NewOutgoingContext(ctx, md)
	return ctx, cancel
}

func (a *GrpcAgent) keepPing(conn *grpc.ClientConn, wg *sync.WaitGroup) {
	defer wg.Done()
	client := v1.NewAgentClient(conn)

	ctx, cancelRequestAgent := a.createMdCtxWithTime(a.config.GrpcConTextTimeOut, a.metadata)
	defer cancelRequestAgent()

	agent_info := common.GetPBAgentInfo(a.agentType, a.config)
	a.log.Debugf("request agentInfo:%v", agent_info)
	if _, err := client.RequestAgentInfo(ctx, agent_info); err != nil {
		a.log.Warnf("create RequestAgentInfo failed with %v", err)
		return
	}

	// send ping
	ping_ctx, cancelPingFunc := a.createMdCtx(a.pingMd)
	defer cancelPingFunc()
	stream, err := client.PingSession(ping_ctx)
	if err != nil {
		a.log.Warnf("create PingSession failed with %v", err)
		return
	}

	defer stream.CloseSend()

	ping := v1.PPing{}
	for {
		// send ping
		a.log.Infof("ping %s %v", a.AgentId, a.pingMd)
		if err := stream.Send(&ping); err != nil {
			a.log.Warnf("agentOnline Send  ping failed. %s", err)
			break
		}
		// recv ping
		if _, err := stream.Recv(); err != nil {
			a.log.Warnf("agentOnline recv ping failed. %s", err)
			break
		}

		if common.WaitEventsWithTime(a.ctx, a.config.PingInterval) == common.E_AGENT_STOPPING {
			break
		}
	}
	a.AgentOnLine = false
}

func (a *GrpcAgent) handleRegisterAgent() error {

	a.log.Infof("connect AgentChannel:%s for agentOnline", a.config.User.AgentAddress)
	conn, err := a.config.CreateGrpcConnection(a.ctx, a.config.User.AgentAddress)
	if err != nil {
		a.log.Warnf("connect %s timeout", a.config.User.AgentAddress)
		return errors.New("connect pinpoint-collector timeout")
	}
	defer conn.Close()

	var cmd_wg sync.WaitGroup
	defer cmd_wg.Wait()

	cmd_wg.Add(1)
	go a.handleCommand(conn, &cmd_wg)

	a.AgentOnLine = true

	cmd_wg.Add(1)
	go a.keepPing(conn, &cmd_wg)

	return nil
}

func (agent *GrpcAgent) keepAgentOnline() {
	defer agent.tasksGroup.Done()

	for {
		if err := agent.handleRegisterAgent(); err != nil {
			agent.log.Infof("agent exit. reason: %s ", err)
		}

		if common.WaitEventsWithTime(agent.ctx, agent.config.AgentReTryTimeout) == common.E_AGENT_STOPPING {
			break
		}
	}
}

func (agent *GrpcAgent) registerFilter() {
	// online/off
	// agent.log.Debug("register agent filter")
	// agent.AddFilter(agent)

	// req count
	agent.log.Debug("register requestCounter filter")
	agent.AddFilter(agent.reqCounter)

	// req UrlTemplateReport
	agent.log.Debug("register UrlTemplate Report filter")
	agent.AddFilter(agent.utReport)

	// req  ErrorAnalysis
	agent.log.Debug("register errorAnalysis Report error")
	errorAnalysisFilter := createErrorAnalysisFilter(agent.ctx, agent.metadata, agent.config, agent.log)
	agent.AddFilter(errorAnalysisFilter)

	// send span
	agent.log.Debug("register spanSender filter")
	spanSender := createSpanSender(agent.metadata, agent.ctx, &agent.tasksGroup, agent.config, agent.log)
	agent.AddFilter(spanSender)

}

func (a *GrpcAgent) CollectPStateMessage() *v1.PStatMessage {

	max, avg := a.reqCounter.GetMaxAvg()
	responseTime := v1.PResponseTime{
		Max: int64(max),
		Avg: int64(avg),
	}

	v, _ := mem.VirtualMemory()

	jvmGc := v1.PJvmGc{
		Type:                 v1.PJvmGcType_JVM_GC_TYPE_PARALLEL,
		JvmMemoryHeapUsed:    int64(v.Used),
		JvmMemoryHeapMax:     int64(v.Total),
		JvmMemoryNonHeapUsed: int64(v.Buffers),
		JvmMemoryNonHeapMax:  int64(v.Cached),
		JvmGcOldCount:        0,
		JvmGcOldTime:         0,
		JvmGcDetailed:        &v1.PJvmGcDetailed{},
	}
	// cpu.Percent calculate cpu in config.StatInterval
	totalPer, err := cpu.PercentWithContext(a.ctx, a.config.StatInterval*time.Second, false)
	totalCpuUsage := 0.0
	if err == nil {
		totalCpuUsage = totalPer[0] / 100
	}

	cpuload := v1.PCpuLoad{
		SystemCpuLoad: totalCpuUsage,
		JvmCpuLoad:    totalCpuUsage,
	}
	var activeTraceCount []int32
	for _, value := range a.reqCounter.GetReqTimeProfiler() {
		activeTraceCount = append(activeTraceCount, int32(value))
	}

	agentStat := v1.PAgentStat{
		ResponseTime:    &responseTime,
		Gc:              &jvmGc,
		CollectInterval: int64(a.config.StatInterval),
		Timestamp:       int64(time.Now().UnixNano() / int64(time.Millisecond)),
		CpuLoad:         &cpuload,
		Transaction:     &v1.PTransaction{},
		ActiveTrace: &v1.PActiveTrace{
			Histogram: &v1.PActiveTraceHistogram{
				Version:             1,
				HistogramSchemaType: 2, //NORMAL SCHEMA
				ActiveTraceCount:    activeTraceCount,
			},
		},
		DataSourceList: nil,
		Deadlock:       nil,
		FileDescriptor: nil,
		DirectBuffer:   nil,
		Metadata:       "",
	}

	pStateAgentStat := v1.PStatMessage_AgentStat{
		AgentStat: &agentStat,
	}

	sateMessage := v1.PStatMessage{
		Field: &pStateAgentStat,
	}

	return &sateMessage
}

func (a *GrpcAgent) handleRequestStat(client v1.Stat_SendAgentStatClient, wg *sync.WaitGroup) {
	defer wg.Done()
	for {
		msg := a.CollectPStateMessage()

		a.log.Debugf("PStatMessage: %v", msg)
		if err := client.Send(msg); err != nil {
			a.log.Warn(err)
			break
		}

		if common.WaitEventsWithTime(a.ctx, 0) == common.E_AGENT_STOPPING {
			break
		}
	}
}

func (agent *GrpcAgent) handleUrlReportStat(client v1.Stat_SendAgentStatClient, wg *sync.WaitGroup) {
	defer wg.Done()
	for {
		msg := agent.utReport.MoveUtReport()

		agent.log.Debugf("ut report:%v", msg)
		if err := client.Send(msg); err != nil {
			agent.log.Warn(err)
			break
		}
		//config.StatInterval
		if common.WaitEventsWithTime(agent.ctx, 30*time.Second) == common.E_AGENT_STOPPING {
			break
		}
	}
}

func (agent *GrpcAgent) sendStat() {

	agent.log.Debugf("connect StatAddress:%s", agent.config.User.StatAddress)

	conn, err := agent.config.CreateGrpcConnection(agent.ctx, agent.config.User.StatAddress)
	if err != nil {
		errorMsg := fmt.Sprintf("Dial %s failed err:%s", agent.config.User.StatAddress, err)
		agent.log.Warn(errorMsg)
		return
	}

	defer conn.Close()

	ctx, cancel := agent.createMdCtx(agent.pingMd)

	defer cancel()

	client := v1.NewStatClient(conn)

	stream, err := client.SendAgentStat(ctx)
	if err != nil {
		agent.log.Warnf("create stat client failed:%s", agent.config.User.StatAddress)
		return
	}

	var wg sync.WaitGroup
	defer wg.Wait()

	wg.Add(1)
	go agent.handleRequestStat(stream, &wg)

	wg.Add(1)
	go agent.handleUrlReportStat(stream, &wg)
}

func (agent *GrpcAgent) uploadStatInfo() {

	defer agent.tasksGroup.Done()

	for {
		agent.sendStat()

		if common.WaitEventsWithTime(agent.ctx, agent.config.StatInterval) == common.E_AGENT_STOPPING {
			return
		}
	}
}

func (agent *GrpcAgent) runBackgroundTasks() {

	agent.registerFilter()

	agent.tasksGroup.Add(1)
	go agent.keepAgentOnline()

	agent.tasksGroup.Add(1)
	go agent.uploadStatInfo()
}

func (agent *GrpcAgent) StartServe() {
	agent.tasksGroup.Add(1)
	go agent.handleTSpanFromBuf()
}

func (agent *GrpcAgent) collectorActiveThreadCount(conn *grpc.ClientConn, responseId int32, interval time.Duration, wg *sync.WaitGroup) {
	defer wg.Done()

	client := v1.NewProfilerCommandServiceClient(conn)
	ctx, _ := common.BuildMdContextWithTimeout(-1, agent.pingMd)

	stream_client, err := client.CommandStreamActiveThreadCount(ctx)
	if err != nil {
		agent.log.Warnf("CommandStreamActiveThreadCount failed:%v", err)
	}
	sequenceId := int32(1)
	for {
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

		for _, value := range agent.reqCounter.GetReqTimeProfiler() {
			res.ActiveThreadCount = append(res.ActiveThreadCount, int32(value))
		}

		res.TimeStamp = time.Now().Unix()
		res.HistogramSchemaType = 2

		if err := stream_client.Send(&res); err != nil {
			agent.log.Warnf("collectorActiveThreadCount:responseId:%d end with:%s", responseId, err)
			break
		}

		if common.WaitEventsWithTime(agent.ctx, interval) == common.E_AGENT_STOPPING {
			agent.log.Warnf("catch exit during send collectorActiveThreadCount")
			break
		}
	}
}

func (agent *GrpcAgent) genCmdHandshake() *v1.PCmdMessage {
	handshake := v1.PCmdServiceHandshake{}
	handshake.SupportCommandServiceKey = append(
		handshake.SupportCommandServiceKey,
		int32(v1.PCommandType_PING),
		int32(v1.PCommandType_PONG),
		int32(v1.PCommandType_ECHO),
		int32(v1.PCommandType_ACTIVE_THREAD_COUNT),
		int32(v1.PCommandType_ACTIVE_THREAD_DUMP),
		int32(v1.PCommandType_ACTIVE_THREAD_LIGHT_DUMP),
	)

	return &v1.PCmdMessage{
		Message: &v1.PCmdMessage_HandshakeMessage{
			HandshakeMessage: &handshake,
		},
	}
}

func (agent *GrpcAgent) handleCommand(conn *grpc.ClientConn, wg *sync.WaitGroup) {
	defer wg.Done()

	var cmd_tasks sync.WaitGroup
	defer cmd_tasks.Wait()

	client := v1.NewProfilerCommandServiceClient(conn)
	//config.AgentReTryTimeout
	ctx, cancel_func := agent.createMdCtx(agent.pingMd)
	defer cancel_func()

	//TODO update HandleCommand to HandleCommandV2
	commandClient, err := client.HandleCommand(ctx)

	if err != nil {
		agent.log.Warnf("handleCommand got err:%s", err)
		return
	}

	// send handle shake
	if err := commandClient.Send(agent.genCmdHandshake()); err != nil {
		agent.log.Warnf("handleCommand Send got err:%s", err)
		return
	}

	agent.log.Debugf("send command handshake %s", agent.genCmdHandshake())
	for {
		cmd, err := commandClient.Recv()
		if err != nil {
			agent.log.Infof("handleCommand.Recv got err:%s", err)
			return
		}

		agent.log.Infof("appid:%s handleCommand: get cmd %s", agent.AgentId, cmd)
		switch cmd.Command.(type) {
		case *v1.PCmdRequest_CommandEcho:
			agent.log.Debug("PCmdRequest_CommandEcho")
		case *v1.PCmdRequest_CommandActiveThreadCount:
			// create a new coro to send active thread
			agent.log.Debug("PCmdRequest_CommandActiveThreadCount")
			cmd_tasks.Add(1)
			go agent.collectorActiveThreadCount(conn, cmd.RequestId, 1, &cmd_tasks)
		case *v1.PCmdRequest_CommandActiveThreadDump:
			agent.log.Debug("PCmdRequest_CommandActiveThreadDump")
		case *v1.PCmdRequest_CommandActiveThreadLightDump:
			agent.log.Debug("PCmdRequest_CommandActiveThreadLightDump")
			response := v1.PCmdResponse{
				ResponseId: cmd.RequestId,
			}

			dumpRes := v1.PCmdActiveThreadLightDumpRes{
				Type:           "java",
				SubType:        "oracle",
				Version:        "1.8.105",
				CommonResponse: &response,
			}

			if _, err := client.CommandActiveThreadLightDump(ctx, &dumpRes); err != nil {
				agent.log.Warnf("CommandActiveThreadLightDump failed! err:%s", err)
			}

		default:
			agent.log.Warnf("unknown command type %v", cmd)
		}
	}

}

func (agent *GrpcAgent) handleTSpanFromBuf() {
	defer agent.tasksGroup.Done()
	for {
		select {
		case span := <-agent.tSpanBufCh:
			for _, filter := range agent.spanFilters {
				if !filter.Interceptor(span) {
					break
				}
			}
		case <-agent.ctx.Done():
			agent.log.Warn("consumeJsonSpan task done, as agent exit")
			return
		}
	}
}

func (agent *GrpcAgent) CheckValid(span *TSpan) bool {
	if span.GetAppName() != agent.agentName || span.GetAppServerType() != agent.agentType {
		agent.log.Warn("name or FT not equal")
		return false
	} else {
		return true
	}

}
