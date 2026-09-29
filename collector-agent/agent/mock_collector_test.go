package agent

import (
	"context"
	"io"
	"net"
	"sync/atomic"
	"testing"
	"time"

	"github.com/pinpoint-apm/pinpoint-c-agent/collector-agent/common"
	v1 "github.com/pinpoint-apm/pinpoint-c-agent/collector-agent/pinpoint-grpc-idl-go/proto/v1"
	"github.com/sirupsen/logrus"
	"google.golang.org/grpc"
	"google.golang.org/grpc/credentials/insecure"
)

// mockCollector is an in-process fake Pinpoint Collector implementing all four
// gRPC services the collector-agent talks to (Agent, Span, Metadata, Stat) plus
// ProfilerCommandService. It lets tests exercise the agent end-to-end without a
// real backend.
type mockCollector struct {
	v1.UnimplementedAgentServer
	v1.UnimplementedSpanServer
	v1.UnimplementedMetadataServer
	v1.UnimplementedStatServer
	v1.UnimplementedProfilerCommandServiceServer

	// issueATC, when true, makes HandleCommand issue one ACTIVE_THREAD_COUNT
	// command after the handshake.
	issueATC bool

	// counters (atomic for concurrent handlers)
	agentInfoCalls atomic.Int64
	pingSessions   atomic.Int64
	spansReceived  atomic.Int64
	statsReceived  atomic.Int64
	metaReceived   atomic.Int64
	atcReceived    atomic.Int64
	atcCommands    atomic.Int64
}

func (m *mockCollector) RequestAgentInfo(ctx context.Context, in *v1.PAgentInfo) (*v1.PResult, error) {
	m.agentInfoCalls.Add(1)
	return &v1.PResult{Success: true}, nil
}

func (m *mockCollector) PingSession(stream v1.Agent_PingSessionServer) error {
	m.pingSessions.Add(1)
	for {
		if _, err := stream.Recv(); err != nil {
			return nil
		}
		if err := stream.Send(&v1.PPing{}); err != nil {
			return nil
		}
	}
}

func (m *mockCollector) SendSpan(stream v1.Span_SendSpanServer) error {
	for {
		if _, err := stream.Recv(); err != nil {
			return nil
		}
		m.spansReceived.Add(1)
	}
}

func (m *mockCollector) SendAgentStat(stream v1.Stat_SendAgentStatServer) error {
	for {
		if _, err := stream.Recv(); err != nil {
			return nil
		}
		m.statsReceived.Add(1)
	}
}

func (m *mockCollector) RequestApiMetaData(ctx context.Context, in *v1.PApiMetaData) (*v1.PResult, error) {
	m.metaReceived.Add(1)
	return &v1.PResult{Success: true}, nil
}

func (m *mockCollector) RequestSqlUidMetaData(ctx context.Context, in *v1.PSqlUidMetaData) (*v1.PResult, error) {
	m.metaReceived.Add(1)
	return &v1.PResult{Success: true}, nil
}

func (m *mockCollector) RequestSqlMetaData(ctx context.Context, in *v1.PSqlMetaData) (*v1.PResult, error) {
	m.metaReceived.Add(1)
	return &v1.PResult{Success: true}, nil
}

func (m *mockCollector) RequestStringMetaData(ctx context.Context, in *v1.PStringMetaData) (*v1.PResult, error) {
	m.metaReceived.Add(1)
	return &v1.PResult{Success: true}, nil
}

func (m *mockCollector) RequestExceptionMetaData(ctx context.Context, in *v1.PExceptionMetaData) (*v1.PResult, error) {
	m.metaReceived.Add(1)
	return &v1.PResult{Success: true}, nil
}

func (m *mockCollector) HandleCommand(stream v1.ProfilerCommandService_HandleCommandServer) error {
	// Read the agent handshake.
	if _, err := stream.Recv(); err != nil {
		return err
	}
	if m.issueATC {
		m.atcCommands.Add(1)
		if err := stream.Send(&v1.PCmdRequest{
			RequestId: 1,
			Command: &v1.PCmdRequest_CommandActiveThreadCount{
				CommandActiveThreadCount: &v1.PCmdActiveThreadCount{},
			},
		}); err != nil {
			return err
		}
	}
	<-stream.Context().Done()
	return nil
}

func (m *mockCollector) CommandStreamActiveThreadCount(stream v1.ProfilerCommandService_CommandStreamActiveThreadCountServer) error {
	for {
		if _, err := stream.Recv(); err != nil {
			return nil
		}
		m.atcReceived.Add(1)
	}
}

// startMockCollector starts the fake collector on a random local port and
// returns its address plus a stop function.
func startMockCollector(t *testing.T, m *mockCollector) (string, func()) {
	t.Helper()
	lis, err := net.Listen("tcp", "127.0.0.1:0")
	if err != nil {
		t.Fatalf("listen failed: %v", err)
	}
	gs := grpc.NewServer()
	v1.RegisterAgentServer(gs, m)
	v1.RegisterSpanServer(gs, m)
	v1.RegisterMetadataServer(gs, m)
	v1.RegisterStatServer(gs, m)
	v1.RegisterProfilerCommandServiceServer(gs, m)
	go gs.Serve(lis)
	return lis.Addr().String(), gs.Stop
}

// mockConfig points every backend address at addr and silences logs.
func mockConfig(addr string) *common.Config {
	config := common.CreateTestConfig()
	config.User.AgentAddress = addr
	config.User.SpanAddress = addr
	config.User.StatAddress = addr
	config.GrpcConTextTimeOut = 2 * time.Second
	config.GrpcOption = []grpc.DialOption{
		grpc.WithTransportCredentials(insecure.NewCredentials()),
		grpc.WithBlock(),
	}
	l := logrus.New()
	l.SetOutput(io.Discard)
	config.Log = l
	config.LogEntry = l.WithField("id", "mock-test")
	return config
}

// TestMockCollectorFullLifecycle exercises the agent against a mock collector:
// registration (RequestAgentInfo + PingSession), span forwarding, and periodic
// AgentStat. It also verifies R3 (regular stats actually flow) end-to-end.
func TestMockCollectorFullLifecycle(t *testing.T) {
	m := &mockCollector{}
	addr, stop := startMockCollector(t, m)
	defer stop()

	config := mockConfig(addr)
	config.StatInterval = 1 * time.Second // speed up the stat loop for the test

	startTime := "1790570125993486"
	agent := CreateGrpcAgent("cd.dev.test.mock", "cd.dev.test.mock", 1500, 1, startTime, config)
	agent.StartServe()
	defer agent.Stop()

	// Wait for registration to complete.
	deadline := time.Now().Add(5 * time.Second)
	for m.agentInfoCalls.Load() == 0 {
		if time.Now().After(deadline) {
			t.Fatal("agent did not register (RequestAgentInfo) in time")
		}
		time.Sleep(20 * time.Millisecond)
	}

	// Send a span and verify it reaches the mock collector.
	span := &TSpan{
		AppServerType: 1500, AppServerTypeV2: 1500,
		AppId: "cd.dev.test.mock", AppIdV2: "cd.dev.test.mock",
		AppName: "cd.dev.test.mock", AppNameV2: "cd.dev.test.mock",
		StartTimeV2: time.Now().Unix(), ElapsedTime: 100, ElapsedTimeV2: 100,
		SpanName: "mock-main", SpanId: 1, ServerType: 1500,
		TransactionId: "cd.dev.test.mock^1790570125993^1",
		Uri:           "/", RemoteAddr: "127.0.0.1", EndPoint: "localhost:5265",
	}
	agent.SendSpan(span)

	deadline = time.Now().Add(5 * time.Second)
	for m.spansReceived.Load() == 0 {
		if time.Now().After(deadline) {
			t.Fatal("span was not forwarded to the mock collector in time")
		}
		time.Sleep(20 * time.Millisecond)
	}

	// Verify periodic AgentStat flows (R3: with the interval bug this would
	// never arrive).
	deadline = time.Now().Add(6 * time.Second)
	for m.statsReceived.Load() == 0 {
		if time.Now().After(deadline) {
			t.Fatal("no AgentStat received — regular stats are not flowing (R3 regression?)")
		}
		time.Sleep(50 * time.Millisecond)
	}

	t.Logf("registered=%d pingSessions=%d spans=%d stats=%d meta=%d",
		m.agentInfoCalls.Load(), m.pingSessions.Load(),
		m.spansReceived.Load(), m.statsReceived.Load(), m.metaReceived.Load())
}

// TestMockCollectorActiveThreadCountRate verifies R10 against a mock collector
// that issues one ACTIVE_THREAD_COUNT command: the agent must emit at most a
// handful of messages per second (1s interval), not hundreds of thousands.
func TestMockCollectorActiveThreadCountRate(t *testing.T) {
	m := &mockCollector{issueATC: true}
	addr, stop := startMockCollector(t, m)
	defer stop()

	config := mockConfig(addr)
	startTime := "1790570125993486"
	agent := CreateGrpcAgent("cd.dev.test.atc.mock", "cd.dev.test.atc.mock", 1500, 1, startTime, config)
	defer agent.Stop()

	// Wait until the command has been issued.
	deadline := time.Now().Add(5 * time.Second)
	for m.atcCommands.Load() == 0 {
		if time.Now().After(deadline) {
			t.Fatal("mock collector did not issue ACTIVE_THREAD_COUNT in time")
		}
		time.Sleep(20 * time.Millisecond)
	}

	const window = 1500 * time.Millisecond
	time.Sleep(window)
	got := m.atcReceived.Load()
	rate := float64(got) / window.Seconds()
	t.Logf("received %d active-thread-count messages in %v (%.0f msg/s)", got, window, rate)

	// With a 1s interval, ~1-2 messages are expected in a 1.5s window.
	if got > 5 {
		t.Errorf("BUG: %d messages in %v (%.0f msg/s) — interval is effectively 1ns", got, window, rate)
	}
}
