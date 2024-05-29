package common

import (
	"context"
	"errors"
	"fmt"
	"strconv"
	"strings"
	"time"

	v1 "github.com/pinpoint-apm/pinpoint-c-agent/collector-agent/pinpoint-grpc-idl/proto/v1"
	"google.golang.org/grpc"
	"google.golang.org/grpc/metadata"
)

type E_AGENT_STATUS int32

const (
	E_AGENT_STOPPING E_AGENT_STATUS = 0
	E_AGENT_GOON     E_AGENT_STATUS = 1
)

func ParseStringField(v string) map[string]string {
	ret := map[string]string{}
	for _, item := range strings.Split(v, " ") {
		values := strings.SplitN(item, "=", 2)
		if len(values) == 2 {
			ret[values[0]] = values[1]
		}
	}
	return ret
}

func ParseDotFormatToTime(v string) (int64, error) {
	ar := strings.Split(v, ".")
	if sec, err := strconv.ParseInt(ar[0], 10, 32); err == nil {
		if ms, err := strconv.ParseInt(ar[1], 10, 32); err == nil {
			return sec*1000 + ms, nil
		}
	}
	return 0, errors.New(fmt.Sprintf("input:%s is invalid", v))
}

func WaitChannelEvent(doneCh chan bool, sec time.Duration) E_AGENT_STATUS {
	select {
	case <-doneCh:
		return E_AGENT_STOPPING
	case <-time.After(sec * time.Second):
		return E_AGENT_GOON
	}
}

func BuildPinpointCtx(sec time.Duration, md metadata.MD) (ctx context.Context, cancel context.CancelFunc) {
	if sec == -1 {
		ctx = context.Background()
		//cancel = nil
		ctx = metadata.NewOutgoingContext(ctx, md)
		return ctx, nil
	} else {
		ctx, cancel = context.WithTimeout(context.Background(), sec*time.Second)
		ctx = metadata.NewOutgoingContext(ctx, md)
		return ctx, cancel
	}

}

func CreateGrpcConnection(address string) (*grpc.ClientConn, error) {
	ctx, cancel := context.WithTimeout(context.Background(), time.Second*config.GrpcConTextTimeOutSec)
	defer cancel()
	conn, err := grpc.DialContext(ctx, address, GetDialOption()...)
	if err != nil {
		return nil, err
	}
	return conn, nil
}

func TypeV1_String_TransactionId(tid_s string) *v1.PTransactionId {
	tidFormat := strings.Split(tid_s, "^")
	if len(tidFormat) < 3 {
		return nil
	}
	agentId, startTime, sequenceId := tidFormat[0], tidFormat[1], tidFormat[2]
	transactionId := &v1.PTransactionId{AgentId: agentId}

	if value, err := strconv.ParseInt(startTime, 10, 64); err == nil {
		transactionId.AgentStartTime = value
	}

	if value, err := strconv.ParseInt(sequenceId, 10, 64); err == nil {
		transactionId.Sequence = value
	}
	return transactionId
}
