package server

import (
	"encoding/binary"
	"encoding/json"
	"fmt"
	"math"
	"net"
	"os"
	"os/signal"
	"strconv"
	"sync"
	"sync/atomic"
	"syscall"
	"time"

	"github.com/pinpoint-apm/pinpoint-c-agent/collector-agent/agent"
	"github.com/pinpoint-apm/pinpoint-c-agent/collector-agent/common"

	"github.com/sirupsen/logrus"
)

// Version is hardcoded here. Do NOT override it via -ldflags at build time;
// the makefile no longer injects it dynamically.
// NOTE: must keep the "v" prefix — the client (pinpointPy C++ core
// HandleHelloMsg) compares this string against lowest_version="v0.7.0"
// lexicographically; without the "v" prefix ("0.7.9" < "v0.7.0") the client
// rejects the handshake and drops the connection.
var Version = "v0.7.9"

type Server struct {
	listener        net.Listener
	agentRouter     agent.I_PacketRouter
	createTime      int64
	uniqueIDCounter int64
	lastTime        atomic.Int64
	config          *common.Config
	log             *logrus.Logger

	// wg tracks client handler goroutines so shutdown can wait for them.
	wg sync.WaitGroup
	// connMu guards conns during shutdown.
	connMu sync.Mutex
	// conns holds the currently served client connections so they can be
	// closed on shutdown (unblocking their reads).
	conns map[net.Conn]struct{}
	// listenerMu guards listener, which is written by startListen and read by
	// shutdown.
	listenerMu sync.Mutex
	// shuttingDown is set once a termination signal is received; the accept
	// loop then stops admitting new connections.
	shuttingDown atomic.Bool
}

func CreateServer(config *common.Config) *Server {
	return &Server{
		config:          config,
		log:             config.Log,
		agentRouter:     agent.CreateAgentRouter(config),
		createTime:      time.Now().Unix(),
		uniqueIDCounter: 0,
		conns:           make(map[net.Conn]struct{}),
	}
}

type ServerInfo struct {
	AppId     string `json:"appid"`
	AppName   string `json:"appname"`
	StartTime string `json:"time"`
	Version   string `json:"version"`
}

type ServerUniqueId struct {
	UID int64 `json:"uid"`
}

const CLIENT_HEADER_SIZE = 8

func (s *Server) Run() (code int, err error) {

	s.wg.Add(1)
	go func() {
		defer s.wg.Done()
		s.startListen()
	}()

	sig := make(chan os.Signal, 1)

	signal.Notify(sig,
		syscall.SIGHUP,
		syscall.SIGINT,
		syscall.SIGTERM,
		syscall.SIGQUIT)

	for {
		select {
		case sig := <-sig:
			s.log.Warnf("catch signal %s", sig)
			s.shutdown()
			return 0, fmt.Errorf("SpanServer exit with signal %s", sig)
		case <-time.After(s.config.AgentRetireTime):
			s.agentRouter.Clean()
		}
	}
}

// shutdown performs an orderly stop: it stops admitting new connections,
// closes the listener and all in-flight client connections (unblocking their
// reads), waits a bounded time for handlers to drain, then stops the router
// agents so their queues and goroutines are released.
func (s *Server) shutdown() {
	s.log.Warn("Stopping listener ...")

	// 1. Stop accepting new connections and close the listener.
	s.shuttingDown.Store(true)
	s.closeListener()

	// 2. Close all in-flight client connections so blocked reads return.
	s.connMu.Lock()
	for conn := range s.conns {
		conn.Close()
	}
	s.connMu.Unlock()

	// 3. Wait (bounded) for client handlers to finish.
	done := make(chan struct{})
	go func() {
		s.wg.Wait()
		close(done)
	}()

	select {
	case <-done:
		s.log.Warn("all client connections drained")
	case <-time.After(s.config.SpanTimeWait):
		s.log.Warnf("shutdown timed out after %v waiting for client connections", s.config.SpanTimeWait)
	}

	// 4. Stop all router agents so their queues/goroutines are released.
	s.agentRouter.Stop()
	s.log.Warn("collector-agent shutdown complete")
}

func (server *Server) genUniqueId() *ServerUniqueId {
	return &ServerUniqueId{
		UID: server.createTime + atomic.AddInt64(&server.uniqueIDCounter, 1),
	}
}

func (s *Server) parsePacket(con net.Conn, size, packetType uint32, body []byte) (err error) {
	if len(body) > 256 {
		s.log.Tracef("size:%d  packetType:%d body:%s ...", size, packetType, string(body[:256]))
	} else {
		s.log.Tracef("size:%d  packetType:%d body:%s ", size, packetType, string(body))
	}

	//todo parse packetType
	// data := make([]byte, size)
	// copy(data, body)
	rawPacket := agent.RawPacket{Type: packetType, RawData: body}

	switch packetType {
	case 1: //REQ_UPDATE_SPAN
		err = s.agentRouter.DispatchPacket(&rawPacket)
		if err != nil {
			s.log.Warnf("dispatcher packet with an exception: %s", err)
			return err
		}
	case 2: //REQ_UNIQUE_ID
		var uniqueBody []byte
		uniqueBody, err = json.Marshal(s.genUniqueId())
		if err == nil {
			err = s.respToClient(con, 2, uniqueBody)
		}
		s.log.Infof("get genUniqueId")
		return err
	default:
		s.log.Warnf("unsupported type:%d", packetType)
	}

	return nil
}

func (s *Server) startListen() {
	socket_type, address := s.config.ParseServerAddress()
	s.log.Debugf("bind server on %v:%s", socket_type, address)
	listener, err := net.Listen(socket_type, address)
	if err != nil {
		s.log.Errorf("bind %s:%s failed with %v", socket_type, address, err)
		panic(err)
	}

	s.setListener(listener)

	// connSem caps the number of concurrently served connections. Each
	// connection holds a RecvBufSize buffer plus a goroutine, so without a
	// limit an unbounded accept loop can exhaust memory/FDs. A slot is taken
	// after accept and released when handleClient returns; when full, the new
	// connection is rejected immediately instead of blocking the accept loop
	// (which would also stall shutdown).
	maxConns := s.config.User.MaxConnections
	if maxConns <= 0 {
		maxConns = common.DefaultMaxConnections
	}
	connSem := make(chan struct{}, maxConns)

	for {
		conn, err := listener.Accept()
		if err != nil {
			if s.shuttingDown.Load() {
				s.log.Warn("listener closed, stop accepting")
			} else {
				s.log.Errorf("accepter failed with %s", err.Error())
			}
			break
		}

		// Reject new connections once shutdown has started.
		if s.shuttingDown.Load() {
			conn.Close()
			continue
		}

		select {
		case connSem <- struct{}{}:
			s.trackConn(conn)
			s.wg.Add(1)
			go func() {
				defer s.wg.Done()
				defer func() { <-connSem }()
				defer s.untrackConn(conn)
				s.handleClient(conn)
			}()
		default:
			s.log.Warnf("connection limit reached(%d), rejecting client:%s", maxConns, conn.RemoteAddr())
			conn.Close()
		}
	}

}

// setListener stores the listener under lock.
func (s *Server) setListener(l net.Listener) {
	s.listenerMu.Lock()
	s.listener = l
	s.listenerMu.Unlock()
}

// closeListener closes the listener (if any) under lock.
func (s *Server) closeListener() {
	s.listenerMu.Lock()
	if s.listener != nil {
		s.listener.Close()
	}
	s.listenerMu.Unlock()
}

// getListener returns the current listener under lock.
func (s *Server) getListener() net.Listener {
	s.listenerMu.Lock()
	defer s.listenerMu.Unlock()
	return s.listener
}

// trackConn registers a client connection so shutdown can close it.
func (s *Server) trackConn(conn net.Conn) {
	s.connMu.Lock()
	s.conns[conn] = struct{}{}
	s.connMu.Unlock()
}

// untrackConn removes a client connection from the tracking set.
func (s *Server) untrackConn(conn net.Conn) {
	s.connMu.Lock()
	delete(s.conns, conn)
	s.connMu.Unlock()
}

func ParseHeader(buffer []byte) (packetLen, packetType uint32) {

	packetType = binary.BigEndian.Uint32(buffer[0:4])
	packetLen = binary.BigEndian.Uint32(buffer[4:8])
	return packetLen, packetType
}

func (server *Server) matchFullPacket(buffer []byte, start int32, total int32, packetLen, packetType *int32, body *[]byte) (token, needs int32) {
	// fetch header
	if total < CLIENT_HEADER_SIZE {
		return 0, CLIENT_HEADER_SIZE - total
	}
	bodyLen, Type := ParseHeader(buffer[start : start+total])
	if bodyLen > uint32(server.config.User.RecvBufSize) {
		return 0, 0
	}

	// fetch packet
	if total < int32(bodyLen+CLIENT_HEADER_SIZE) {
		return 0, int32(bodyLen) + CLIENT_HEADER_SIZE - total
	}
	*packetType = int32(Type)
	*packetLen = int32(bodyLen)
	*body = buffer[start+CLIENT_HEADER_SIZE : start+CLIENT_HEADER_SIZE+int32(bodyLen)]

	return int32(bodyLen) + CLIENT_HEADER_SIZE, 0
}

func (s *Server) respToClient(con net.Conn, msgType uint32, msgBody []byte) error {
	if len(msgBody) > math.MaxInt32 {
		s.log.Warnf("gets large message(=%d),server skip send to client", len(msgBody))
		return nil
	}

	buffer := make([]byte, 8)
	binary.BigEndian.PutUint32(buffer[0:4], msgType)
	binary.BigEndian.PutUint32(buffer[4:8], (uint32(len(msgBody))))
	buffer = append(buffer, msgBody...)
	totalSize := 8 + len(msgBody)
	// send handshake message
	for offset := 0; offset < totalSize; {
		size, err := con.Write(buffer[offset:])
		if err != nil {
			return fmt.Errorf("client:%s channel error:%s", con.RemoteAddr(), err)
		}
		if size == 0 {
			return fmt.Errorf("client:%s channel error: zero-byte write", con.RemoteAddr())
		}
		offset += size
	}

	return nil
}

func (server *Server) genHello() *ServerInfo {
	info := &ServerInfo{
		AppId:   "no",
		AppName: "no",
		Version: Version,
	}
	for {
		now_in_ms := time.Now().UnixMilli()
		if now_in_ms == server.lastTime.Load() {
			// force sleep 1ms,avoiding conflict
			time.Sleep(1 * time.Microsecond)
			continue
		}
		server.lastTime.Store(now_in_ms)
		info.StartTime = strconv.FormatInt(now_in_ms, 10)
		break
	}
	return info
}

func (s *Server) handleClient(con net.Conn) {
	defer con.Close()
	// 	if err := con.Close(); err != nil {
	// 		s.log.Warnf("close client met :%s", err)
	// 	}
	// }()

	s.log.Infof("client:%s is online", con.RemoteAddr())
	handshake, err := json.Marshal(s.genHello())
	if err != nil {
		s.log.Warnf("generate handshake failed.reason:%s", err)
		return
	}
	s.log.Infof("send handshake msg:%s", handshake)
	//skip error checking,as con.Read does
	s.respToClient(con, 0, handshake)

	// fetch data
	clientInBuf := make([]byte, s.config.User.RecvBufSize)
	inOffset := 0
	packetOffset := 0

	for {

		size, err := con.Read(clientInBuf[inOffset:])

		if err != nil {
			s.log.Warnf("client:%s read error:%s", con.RemoteAddr(), err)
			break
		}

		if size > 0 {
			inOffset += size
		} else if size == 0 {
			s.log.Infof("Connection:%s is closed by client. Reason: read size:%d in_offset:%d packet_offset:%d", con.RemoteAddr(), size, inOffset, packetOffset)
			break
		}

	ParseAgain:

		var body []byte = nil
		var packetLen, packetType int32

		token, needs := s.matchFullPacket(clientInBuf, int32(packetOffset), int32(inOffset-packetOffset), &packetLen, &packetType, &body)
		if token == 0 {
			if needs == 0 {
				s.log.Errorf("oversized packet rejected: bodyLen exceeds RecvBufSize(%d). client:%s", s.config.User.RecvBufSize, con.RemoteAddr())
				break
			}

			if s.config.User.RecvBufSize-inOffset < int(needs) {
				// not enough space to hold income
				if int(needs) > s.config.User.RecvBufSize/2 {
					s.log.Errorf("packet overflow and overlap.Reason packet_offset:%d,in_offset:%d", packetOffset, inOffset)
					break
				}
				unParsedSize := inOffset - packetOffset
				copy(clientInBuf[0:unParsedSize], clientInBuf[packetOffset:inOffset])
				inOffset = unParsedSize
				packetOffset = 0
			}
			continue
		}

		packetOffset += int(token)

		// gets a packet
		err = s.parsePacket(con, uint32(packetLen), uint32(packetType), body)
		if err != nil {
			s.log.Warnf("parsePacket catches error:%s,client:%s", err, con.RemoteAddr())
			break
		}

		if packetOffset == inOffset {
			packetOffset = 0
			inOffset = 0
			s.log.Debug("ring buffer back to start")
			continue
		}

		goto ParseAgain

	}

	s.log.Infof("connection:%s is shutdown", con.RemoteAddr())
}
