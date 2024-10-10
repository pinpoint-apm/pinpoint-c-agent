package agent

import (
	"context"
	"sync"
	"testing"

	"github.com/pinpoint-apm/pinpoint-c-agent/collector-agent/common"
)

func TestSqlUidFormat(t *testing.T) {
	var wg sync.WaitGroup
	config := common.CreateTestConfig()
	sender := createSpanSender(nil, context.Background(), &wg, config, config.LogEntry)

	id := sender.getSqlUidMetaApiId("INSERT INTO chengji_m VALUES (%s, %s, %s)")
	t.Logf("%v", string(id))
	if len(id) == 0 {
		t.Error(id)
	}
}
