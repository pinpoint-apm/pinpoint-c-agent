package agent

import (
	"context"
	"net"
	"sync"
	"sync/atomic"
	"testing"
	"time"

	"github.com/pinpoint-apm/pinpoint-c-agent/collector-agent/common"
	v1 "github.com/pinpoint-apm/pinpoint-c-agent/collector-agent/pinpoint-grpc-idl-go/proto/v1"
	"google.golang.org/grpc"
	"google.golang.org/grpc/credentials/insecure"
	"google.golang.org/grpc/metadata"
)

// fakeCmdServer is a local stand-in for the Pinpoint Collector's
// ProfilerCommandService. It sends one ACTIVE_THREAD_COUNT command and counts
// how many PCmdActiveThreadCountRes messages the agent pushes back.
type fakeCmdServer struct {
	v1.UnimplementedProfilerCommandServiceServer
	atcCount atomic.Int64
}

func (s *fakeCmdServer) HandleCommand(stream v1.ProfilerCommandService_HandleCommandServer) error {
	// Read the agent's handshake.
	if _, err := stream.Recv(); err != nil {
		return err
	}
	// Issue a single ACTIVE_THREAD_COUNT command.
	if err := stream.Send(&v1.PCmdRequest{
		RequestId: 1,
		Command: &v1.PCmdRequest_CommandActiveThreadCount{
			CommandActiveThreadCount: &v1.PCmdActiveThreadCount{},
		},
	}); err != nil {
		return err
	}
	<-stream.Context().Done()
	return nil
}

func (s *fakeCmdServer) CommandStreamActiveThreadCount(stream v1.ProfilerCommandService_CommandStreamActiveThreadCountServer) error {
	for {
		if _, err := stream.Recv(); err != nil {
			return nil
		}
		s.atcCount.Add(1)
	}
}

// TestLocalActiveThreadCountRate measures how many active-thread-count messages
// the agent emits after a single ACTIVE_THREAD_COUNT command. With the literal
// interval `1` (= 1ns) the loop is effectively unthrottled; with a 1s interval
// only a handful should arrive in the measurement window.
func TestLocalActiveThreadCountRate(t *testing.T) {
	lis, err := net.Listen("tcp", "127.0.0.1:0")
	if err != nil {
		t.Fatalf("listen failed: %v", err)
	}
	gs := grpc.NewServer()
	fake := &fakeCmdServer{}
	v1.RegisterProfilerCommandServiceServer(gs, fake)
	go gs.Serve(lis)
	defer gs.Stop()

	config := common.CreateTestConfig()
	config.User.AgentAddress = lis.Addr().String()
	config.GrpcConTextTimeOut = 2 * time.Second
	config.GrpcOption = []grpc.DialOption{
		grpc.WithTransportCredentials(insecure.NewCredentials()),
		grpc.WithBlock(),
	}

	conn, err := grpc.Dial(lis.Addr().String(),
		grpc.WithTransportCredentials(insecure.NewCredentials()),
		grpc.WithBlock(),
		grpc.WithTimeout(2*time.Second))
	if err != nil {
		t.Fatalf("dial failed: %v", err)
	}
	defer conn.Close()

	ctx, cancel := context.WithCancel(context.Background())
	defer cancel()

	a := &GrpcAgent{
		config:     config,
		ctx:        ctx,
		cancel_ctx: cancel,
		log:        config.LogEntry,
		reqCounter: createRequestCounter(config),
		pingMd:     metadata.New(map[string]string{"agentid": "local-test"}),
	}

	var wg sync.WaitGroup
	wg.Add(1)
	go a.handleCommand(conn, &wg)

	// Measurement window.
	const window = 500 * time.Millisecond
	time.Sleep(window)
	got := fake.atcCount.Load()
	cancel()
	wg.Wait()

	rate := float64(got) / window.Seconds()
	t.Logf("received %d active-thread-count messages in %v (%.0f msg/s)", got, window, rate)

	// With a sane interval (>= 1s) we expect at most a couple of messages in a
	// 500ms window. A large count means the loop is unthrottled (1ns interval).
	if got > 5 {
		t.Errorf("BUG REPRODUCED: %d messages in %v (%.0f msg/s) — interval is effectively 1ns", got, window, rate)
	}
}
