package server

import (
	"encoding/binary"
	"encoding/json"
	"errors"
	"fmt"
	"net"
	"os"
	"os/signal"
	"strconv"
	"sync"
	"syscall"
	"time"

	"github.com/pinpoint-apm/pinpoint-c-agent/collector-agent/agent"
	"github.com/pinpoint-apm/pinpoint-c-agent/collector-agent/common"

	log "github.com/sirupsen/logrus"
)

type SpanServer struct {
	listener    net.Listener
	quit        chan bool
	wg          sync.WaitGroup
	agentRouter agent.I_PacketRouter
}

type ServerInfo struct {
	AppId     string `json:"appid"`
	AppName   string `json:"appname"`
	StartTime string `json:"time"`
}

var info = ServerInfo{
	AppId:     "no",
	AppName:   "no",
	StartTime: strconv.FormatInt(time.Now().Unix(), 10),
}

const CLIENT_HEADER_SIZE = 8

// const CLIENT_MAX_PACKET_SIZE = 4096*100 + CLIENT_HEADER_SIZE

func (server *SpanServer) init() {
	server.quit = make(chan bool)
	manager := &agent.AgentRouter{PingId: 1,
		AgentMap: make(map[string]*agent.GrpcAgent),
		Quit:     server.quit,
		WG:       &server.wg,
	}

	server.agentRouter = manager
}

func (server *SpanServer) Run() (code int, err error) {
	server.init()

	go server.startListen()
	sig := make(chan os.Signal, 1)

	signal.Notify(sig,
		syscall.SIGHUP,
		syscall.SIGINT,
		syscall.SIGTERM,
		syscall.SIGQUIT)

	config := common.GetConfig()

	defer func() {
		close(server.quit)
		if err := server.listener.Close(); err != nil {
			log.Warnf("close listener socket met:%s", err)
		}
		server.wg.Wait()
	}()

	for {
		select {
		case sig := <-sig:
			log.Warnf("catch signal %s", sig)
			return 0, errors.New(fmt.Sprintf("SpanServer exit with signal %s", sig))
		case <-time.After(config.AgentFreeOnlineSurvivalTimeSec * time.Second):
			server.agentRouter.Clean()
		}
	}
}

func (server *SpanServer) parseInComePacket(size, packetType uint32, body []byte) (err error) {
	log.Debugf("size: %d  packetType: %d body:%s ", size, packetType, string(body[:]))

	data := make([]byte, size)
	copy(data, body)
	rawPacket := agent.RawPacket{Type: packetType, RawData: data}

	err = server.agentRouter.DispatchPacket(&rawPacket)
	if err != nil {
		log.Warnf("dispather packet with an exception: %s", err)
		return err
	}

	return nil
}

func (server *SpanServer) startListen() {
	defer server.wg.Done()
	server.wg.Add(1)
	server.quit = make(chan bool)
	config := common.GetConfig()
	log.Debug("listen: ", config.SocketType, " address: ", config.Address)
	listener, err := net.Listen(config.SocketType, config.Address)
	if err != nil {
		log.Errorf("bind %s:%s failed with %v", config.SocketType, config.Address, err.Error())
		panic(err)
	}
	server.listener = listener

	for {
		conn, err := listener.Accept()
		if err != nil {
			select {
			case <-server.quit:
				return
			default:
				log.Errorf("accepter failed with %s", err.Error())
			}
		}
		go server.handleClient(conn)
	}

}

func ParseHeader(buffer []byte) (packetLen, packetType uint32) {

	packetType = binary.BigEndian.Uint32(buffer[0:4])
	packetLen = binary.BigEndian.Uint32(buffer[4:8])
	return packetLen, packetType
}

func (server *SpanServer) searchPacket(buffer []byte, start int32, total int32, packetLen, packetType *int32, body *[]byte) (token, needs int32) {
	// fetch header
	if total < CLIENT_HEADER_SIZE {
		return 0, CLIENT_HEADER_SIZE - total
	}
	bodyLen, Type := ParseHeader(buffer[start : start+total])
	if bodyLen > uint32(Setting.RecvBufSize) {
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

func (server *SpanServer) handleClient(con net.Conn) {
	defer func() {
		if err := con.Close(); err != nil {
			log.Warnf("close client met :%s", err)
		}
	}()

	log.Infof("client:%s is online", con.RemoteAddr())
	handshake, err := json.Marshal(&info)
	if err != nil {
		log.Warnf("generate handshake failed.reason:%s", err)
		return
	}
	log.Infof("send handshake msg:%s", handshake)
	msgSize := len(handshake)

	buffer := make([]byte, 8)
	binary.BigEndian.PutUint32(buffer[0:4], (0))
	binary.BigEndian.PutUint32(buffer[4:8], (uint32(msgSize)))

	buffer = append(buffer, handshake...)

	// send handshake message
	for offset := 0; offset < msgSize; {

		size, err := con.Write(buffer[offset:])
		if err != nil {
			log.Warnf("client:%s channel error:%s", con.RemoteAddr(), err)
		}
		offset += size
	}

	// fetch data
	clientInBuf := make([]byte, Setting.RecvBufSize) 
	inOffset := 0
	packetOffset := 0

	for {

		size, err := con.Read(clientInBuf[inOffset:])

		if err != nil {
			log.Warnf("client:%s read error:%s", con.RemoteAddr(), err)
			break
		}

		if size > 0 {
			inOffset += size
		}

		if size == 0 {
			log.Infof("Connection:%s is closed by client. Reason: size:%d in_offset:%d packet_offset:%d", con.RemoteAddr(), size, inOffset, packetOffset)
			break
		}

	ParseAgin:

		var body []byte = nil
		var packetLen, packetType int32

		token, needs := server.searchPacket(clientInBuf, int32(packetOffset), int32(inOffset-packetOffset), &packetLen, &packetType, &body)

		if token == 0 {
			if needs == 0 {
				log.Error("needs cannot be 0")
				break
			}

			if Setting.RecvBufSize -inOffset < int(needs){
				// not enough space to hold income
				if int(needs) > Setting.RecvBufSize / 2 {
					log.Errorf("packet overflow and overlap.Reason packet_offset:%d,in_offset:%d", packetOffset, inOffset)
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

		// get a packet
		err = server.parseInComePacket(uint32(packetLen), uint32(packetType), body)
		if err != nil {
			log.Warnf("parseInComePacket catches error:%s,client:%s", err, con.RemoteAddr())
			break
		}

		if packetOffset == inOffset {
			packetOffset = 0
			inOffset = 0
			log.Debug("ring buffer back to start")
			continue
		}

		goto ParseAgin

	}

	log.Infof("connection:%s is shutdown", con.RemoteAddr())

}
