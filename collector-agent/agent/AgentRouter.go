package agent

import (
	"encoding/json"
	"errors"
	"fmt"
	"strconv"
	"strings"
	"sync"
	"time"

	"github.com/pinpoint-apm/pinpoint-c-agent/collector-agent/common"

	log "github.com/sirupsen/logrus"
)

type RawPacket struct {
	Type    uint32
	RawData []byte
}

type I_PacketRouter interface {
	DispatchPacket(*RawPacket) error
	Clean()
}

type AgentRouter struct {
	AgentMap map[string]*GrpcAgent
	PingId   int32
	Quit     chan bool
	WG       *sync.WaitGroup
	rwMutex  sync.RWMutex
}

type TSpanEvent struct {
	Name            string          `json:"name"`
	ExceptionInfo   string          `json:"EXP,omitempty"`
	ExceptionInfoV2 *TExceptionInfo `json:"EXP_V2,omitempty"`
	DestinationId   string          `json:"dst,omitempty"`
	NextSpanId      int64           `json:"nsid,string,omitempty"`
	EndPoint        string          `json:"server,omitempty"`
	StartElapsed    int32           `json:"S"`
	EndElapsed      int32           `json:"E"`
	StartElapsedV2  int32           `json:":S"`
	EndElapsedV2    int32           `json:":E"`
	ServiceType     int32           `json:"stp,string"`
	Clues           []string        `json:"clues,omitempty"`
	Calls           []*TSpanEvent   `json:"calls,omitempty"`
	SqlMeta         string          `json:"SQL,omitempty"`
}

func (spanEv *TSpanEvent) GetEndElapsed() int32 {
	if spanEv.EndElapsedV2 != 0 {
		return spanEv.EndElapsedV2
	} else {
		return spanEv.EndElapsed
	}
}

func (spanEv *TSpanEvent) GetStartElapsed() int32 {
	if spanEv.StartElapsedV2 != 0 {
		return spanEv.StartElapsedV2
	} else {
		return spanEv.StartElapsed
	}
}

type TErrorInfo struct {
	Msg  string `json:"msg"`
	File string `json:"file"`
	Line int    `json:"line"`
}

type TExceptionInfo struct {
	ClassName string `json:"C"`
	Message   string `json:"M"`
	StartTime int64  `json:":S"`
}

type TSpan struct {
	AppServerType         int32           `json:"FT"`
	AppServerTypeV2       int32           `json:":FT"`
	ParentAppServerType   int32           `json:"ptype,string"`
	ParentSpanId          int64           `json:"psid,string"`
	ParentApplicationName string          `json:"pname"`
	StartTime             int64           `json:"S"`
	StartTimeV2           int64           `json:":S"`
	ElapsedTime           int32           `json:"E"`
	ElapsedTimeV2         int32           `json:":E"`
	AppId                 string          `json:"appid"`
	AppIdV2               string          `json:":appid"`
	AppName               string          `json:"appname"`
	AppNameV2             string          `json:":appname"`
	Calls                 []*TSpanEvent   `json:"calls"`
	Clues                 []string        `json:"clues,omitempty"`
	SpanName              string          `json:"name"`
	SpanId                int64           `json:"sid,string"`
	ServerType            int32           `json:"stp,string"`
	TransactionId         string          `json:"tid"`
	Uri                   string          `json:"uri"`
	UT                    string          `json:"UT,omitempty"`
	EndPoint              string          `json:"server"`
	RemoteAddr            string          `json:"client"`
	AcceptorHost          string          `json:"Ah"`
	ExceptionInfo         string          `json:"EXP,omitempty"`
	ExceptionInfoV2       *TExceptionInfo `json:"EXP_V2,omitempty"`
	ErrorInfo             *TErrorInfo     `json:"ERR,omitempty"`
	ErrorMarked           int32           `json:"EA,omitempty"`
	NginxHeader           string          `json:"NP,omitempty"`
	ApacheHeader          string          `json:"AP,omitempty"`
}

func (span *TSpan) IsFailed() bool {
	if span.ErrorInfo != nil || len(span.ExceptionInfo) > 0 {
		return true
	}
	return false
}

//note
// FindHistogramLevel must come with histogramSize
func (span *TSpan) FindHistogramLevel() int {
	if span.GetElapsedTime() <= 100 {
		return 0
	} else if span.GetElapsedTime() <= 300 {
		return 1
	} else if span.GetElapsedTime() <= 500 {
		return 2
	} else if span.GetElapsedTime() <= 1000 {
		return 3
	} else if span.GetElapsedTime() <= 3000 {
		return 4
	} else if span.GetElapsedTime() <= 5000 {
		return 5
	} else if span.GetElapsedTime() <= 8000 {
		return 6
	} else {
		return 7
	}
}

func (span *TSpan) GetAppServerType() int32 {
	if span.AppServerTypeV2 != 0 {
		return span.AppServerTypeV2
	} else {
		return span.AppServerType
	}
}

func (span *TSpan) GetElapsedTime() int32 {
	if span.ElapsedTimeV2 != 0 {
		return span.ElapsedTimeV2
	} else {
		return span.ElapsedTime
	}
}

func (span *TSpan) GetStartTime() int64 {
	if span.StartTimeV2 != 0 {
		return span.StartTimeV2
	} else {
		return span.StartTime
	}
}

func (span *TSpan) GetAppid() string {
	if len(span.AppIdV2) > 0 {
		return span.AppIdV2
	} else {
		return span.AppId
	}
}

func (span *TSpan) GetAppName() string {
	if len(span.AppNameV2) > 0 {
		return span.AppNameV2
	} else {
		return span.AppName
	}
}

func (manager *AgentRouter) Clean() {
	config := common.GetConfig()
	ctime := time.Now().Unix()
	manager.rwMutex.RLock()
	for id, agent := range manager.AgentMap {
		if agent.GetLastBusyTime()+int64(config.AgentFreeOnlineSurvivalTimeSec) < ctime {
			log.Warnf("agent:%s expired after:%d sec. busyTime:%d", agent, config.AgentFreeOnlineSurvivalTimeSec, agent.GetLastBusyTime())
			manager.rwMutex.RUnlock()
			manager.rwMutex.Lock()
			delete(manager.AgentMap, id)
			agent.Stop()
			manager.rwMutex.Unlock()
			manager.rwMutex.RLock()
		}
	}
	manager.rwMutex.RUnlock()
}

func GetAgentInfo(span *TSpan) (appid, name string, appServerType int32, startTime string, err error) {

	// new feat: get current startTime
	startTime = strconv.FormatInt(common.GetConfig().StartTime, 10) + "000"
	holder := strings.Split(span.TransactionId, "^")
	if len(holder) < 3 {
		log.Warn("tid in wrong format")
	} else if len(holder[1]) <= 10 { // seconds format
		startTime = holder[1] + "000"
	} else { // milliseconds format
		startTime = holder[1]
	}

	appid = span.GetAppid()
	if len(appid) == 0 {
		return "", "", 0, "", errors.New("no appid")
	}

	name = span.GetAppName()

	if len(name) == 0 {
		return "", "", 0, "", errors.New("no appName")
	}

	appServerType = span.GetAppServerType()

	if appServerType == 0 {
		return "", "", 0, "", errors.New("no AppServerType(FT)")
	}

	return appid, name, appServerType, startTime, nil
}

func (manager *AgentRouter) DispatchPacket(packet *RawPacket) error {
	//note: set default var of TSpan
	span := &TSpan{
		// ParentSpanId:-1 is part of logic in pinpoint
		ParentSpanId: -1,
		ErrorMarked:  0,
	}

	if err := json.Unmarshal(packet.RawData, span); err != nil {
		log.Warnf("json.Unmarshal err:%v", err)
		goto PACKET_INVALID
	}

	if appid, appName, serverType, startTime, err := GetAgentInfo(span); err == nil {
		manager.rwMutex.RLock()
		log.Debug("Read-lock is holding")
		agent, OK := manager.AgentMap[appid]
		if !OK {
			// create a new agent
			manager.rwMutex.RUnlock()
			log.Infof("agent:%s not find, create a new agent.", appid)
			log.Debug("Try to get write-lock")
			manager.rwMutex.Lock()
			log.Debug("Write-lock is holding")
			if _t, OK := manager.AgentMap[appid]; OK {
				agent = _t
			} else {
				agent = createGrpcAgent(appid, appName, serverType, manager.PingId, startTime)
				manager.PingId += 1
			}
			manager.AgentMap[appid] = agent
			manager.rwMutex.Unlock()
			log.Debug("Write-lock is release")
		} else {
			manager.rwMutex.RUnlock()
			log.Debug("Read-lock is release")
		}

		agent.CheckValid(span)
		agent.SendSpan(span)
		return nil

	} else {
		log.Warn(err)
		return err
	}

PACKET_INVALID:
	return fmt.Errorf("input packet invalid %s", packet.RawData)
}
