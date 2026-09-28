package server

import (
	"io"
	"net"
	"sync/atomic"
	"testing"
	"time"

	"github.com/pinpoint-apm/pinpoint-c-agent/collector-agent/agent"
	"github.com/pinpoint-apm/pinpoint-c-agent/collector-agent/common"
	"github.com/sirupsen/logrus"
)

// shutdownRouter is a minimal I_PacketRouter that records whether Stop() was
// called, so the shutdown test can assert the router was actually stopped.
type shutdownRouter struct {
	stopped atomic.Bool
}

func (r *shutdownRouter) DispatchPacket(*agent.RawPacket) error { return nil }
func (r *shutdownRouter) Clean()                                {}
func (r *shutdownRouter) Stop()                                 { r.stopped.Store(true) }

func newShutdownTestServer(t *testing.T) (*Server, *shutdownRouter) {
	t.Helper()
	config := common.CreateTestConfig()
	config.User.BindAddress = "127.0.0.1:0" // let the OS pick a free port
	config.SpanTimeWait = 2 * time.Second

	l := logrus.New()
	l.SetOutput(io.Discard)

	router := &shutdownRouter{}
	s := &Server{
		config:      config,
		log:         l,
		agentRouter: router,
		conns:       make(map[net.Conn]struct{}),
	}
	return s, router
}

// TestShutdownClosesClientsAndStopsRouter verifies the R11 fix: on shutdown the
// server stops accepting, closes in-flight connections, waits (bounded) for
// handlers, and stops the router agents.
func TestShutdownClosesClientsAndStopsRouter(t *testing.T) {
	s, router := newShutdownTestServer(t)

	go s.startListen()

	// Wait until the listener is bound.
	deadline := time.Now().Add(2 * time.Second)
	for s.getListener() == nil {
		if time.Now().After(deadline) {
			t.Fatal("listener was not bound in time")
		}
		time.Sleep(10 * time.Millisecond)
	}
	addr := s.getListener().Addr().String()

	// Open a client connection and keep it idle (no data sent).
	conn, err := net.Dial("tcp", addr)
	if err != nil {
		t.Fatalf("dial failed: %v", err)
	}
	defer conn.Close()

	// Drain the handshake so the server-side handler is established.
	conn.SetReadDeadline(time.Now().Add(2 * time.Second))
	head := make([]byte, 8)
	if _, err := io.ReadFull(conn, head); err != nil {
		t.Fatalf("read handshake header failed: %v", err)
	}
	body := make([]byte, int(uint32(head[4])<<24|uint32(head[5])<<16|uint32(head[6])<<8|uint32(head[7])))
	if _, err := io.ReadFull(conn, body); err != nil {
		t.Fatalf("read handshake body failed: %v", err)
	}

	// Shutdown must return promptly (bounded by SpanTimeWait) and stop router.
	done := make(chan struct{})
	go func() {
		s.shutdown()
		close(done)
	}()

	select {
	case <-done:
	case <-time.After(5 * time.Second):
		t.Fatal("shutdown did not return within the bound")
	}

	if !router.stopped.Load() {
		t.Error("router.Stop() was not called during shutdown")
	}

	// The in-flight connection must have been closed by the server.
	conn.SetReadDeadline(time.Now().Add(2 * time.Second))
	buf := make([]byte, 1)
	if _, err := conn.Read(buf); err == nil {
		t.Error("expected in-flight connection to be closed by shutdown")
	}

	// New connections must be rejected after shutdown.
	if c, err := net.Dial("tcp", addr); err == nil {
		c.Close()
		t.Error("expected new connections to be rejected after shutdown")
	}
}

// TestShutdownRejectsNewConnections verifies the shuttingDown flag causes the
// accept loop to stop admitting clients.
func TestShutdownRejectsNewConnections(t *testing.T) {
	s, _ := newShutdownTestServer(t)
	go s.startListen()

	deadline := time.Now().Add(2 * time.Second)
	for s.getListener() == nil {
		if time.Now().After(deadline) {
			t.Fatal("listener was not bound in time")
		}
		time.Sleep(10 * time.Millisecond)
	}

	s.shuttingDown.Store(true)
	s.closeListener()

	// Give the accept loop a moment to observe the closed listener.
	time.Sleep(100 * time.Millisecond)

	if c, err := net.Dial("tcp", s.getListener().Addr().String()); err == nil {
		c.Close()
		t.Error("expected dial to fail after listener closed")
	}
}
