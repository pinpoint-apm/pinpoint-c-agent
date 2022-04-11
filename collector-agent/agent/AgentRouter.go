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

func (manager *AgentRouter) _createAgent(id, name string, agentType int32, startTime string) *GrpcAgent {
	agent := GrpcAgent{PingId: manager.PingId, AgentOnLine: false}
	manager.PingId += 1
	agent.Init(id, name, agentType, startTime)
	agent.Start()

	log.Infof("agent:%v is launched", &agent)
	return &agent
}

func GetAgentInfo(span map[string]interface{}) (id, name string, ft int32, startTime string, err error) {
	if value, OK := span["appid"].(string); !OK {
		return "", "", 0, "", errors.New("no appid")
	} else {
		id = value
	}

	if value, OK := span["appname"].(string); !OK {
		return "", "", 0, "", errors.New("no appname")
	} else {
		name = value
	}

	if value, OK := span["FT"].(float64); !OK {
		return "", "", 0, "", errors.New("no FT")
	} else {
		ft = int32(value)
	}

	// new feat: get current startTime
	if value, OK := span["tid"].(string); OK {
		holder := strings.Split(value, "^")
		if len(holder) < 3 {
			log.Warn("tid in wrong format")
		}
		startTime = holder[1] + "000"
	} else {
		startTime = strconv.FormatInt(common.GetConfig().StartTime, 10) + "000"
	}

	return id, name, ft, startTime, nil
}

func (manager *AgentRouter) DispatchPacket(packet *RawPacket) error {
	var span map[string]interface{}
	if err := json.Unmarshal(packet.RawData, &span); err != nil {
		log.Warnf("Catches unjson-serialized data %s", packet.RawData)
		goto PACKET_INVALIED
	}

	if id, name, ft, startTime, err := GetAgentInfo(span); err == nil {
		manager.rwMutex.RLock()
		log.Debug("Read-lock is holding")
		agent, OK := manager.AgentMap[id]
		if !OK {
			// create a new agent
			manager.rwMutex.RUnlock()
			log.Infof("agent:%s not find, create a new agent.", id)
			log.Debug("Try to get write-lock")
			manager.rwMutex.Lock()
			log.Debug("Write-lock is holding")
			if _t, OK := manager.AgentMap[id]; OK {
				agent = _t
			} else {
				agent = manager._createAgent(id, name, ft, startTime)
			}
			manager.AgentMap[id] = agent

			manager.rwMutex.Unlock()
			log.Debug("Write-lock is release")
		} else {
			manager.rwMutex.RUnlock()
			log.Debug("Read-lock is release")
		}

		agent.CheckValid(name, ft) // CA just checking the name and ft
		agent.SendSpan(span)
		return nil

	} else {
		log.Warn(err)
		return err
	}

PACKET_INVALIED:
	return errors.New(fmt.Sprintf("input packet invalid %s", packet.RawData))
}
