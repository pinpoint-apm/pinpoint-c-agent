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

var Version = "0.7"

type Server struct {
	listener        net.Listener
	agentRouter     agent.I_PacketRouter
	createTime      int64
	uniqueIDCounter int64
	lastTime        int64
	config          *common.Config
	log             *logrus.Logger
}

func CreateServer(config *common.Config) *Server {
	return &Server{
		config:          config,
		log:             config.Log,
		agentRouter:     agent.CreateAgentRouter(config),
		createTime:      time.Now().Unix(),
		uniqueIDCounter: 0,
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

	var wg sync.WaitGroup
	defer wg.Wait()
	wg.Add(1)

	go func() {
		defer wg.Done()
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
			s.log.Warn("Stopping listener ...")
			s.listener.Close()
			return 0, fmt.Errorf("SpanServer exit with signal %s", sig)
		case <-time.After(s.config.AgentRetireTime):
			s.agentRouter.Clean()
		}
	}
}

func (server *Server) genUniqueId() *ServerUniqueId {
	return &ServerUniqueId{
		UID: server.createTime + atomic.AddInt64(&server.uniqueIDCounter, 1),
	}
}

func (s *Server) parsePacket(con net.Conn, size, packetType uint32, body []byte) (err error) {
	s.log.Debugf("size:%d  packetType:%d body:%s ", size, packetType, string(body[:]))

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
		uniqueBody, err := json.Marshal(s.genUniqueId())
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
	var wg sync.WaitGroup

	socket_type, address := s.config.ParseServerAddress()
	s.log.Debugf("bind server on %v:%s", socket_type, address)
	listener, err := net.Listen(socket_type, address)
	if err != nil {
		s.log.Errorf("bind %s:%s failed with %v", socket_type, address, err)
		panic(err)
	}

	s.listener = listener

	for {
		conn, err := listener.Accept()
		if err != nil {
			s.log.Errorf("accepter failed with %s", err.Error())
			break
		}
		wg.Add(1)
		go func() {
			defer wg.Done()
			s.handleClient(conn)
		}()
	}

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
		if now_in_ms == server.lastTime {
			// force sleep 1ms,avoiding conflict
			time.Sleep(1 * time.Microsecond)
			continue
		}
		server.lastTime = now_in_ms
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
				s.log.Error("needs cannot be 0")
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
