package agent

import (
	"sync"
	"testing"

	log "github.com/sirupsen/logrus"
)

func TestSqlUidFormat(t *testing.T) {
	// test  chan
	exitCh := make(chan bool) // chan bool
	var wg sync.WaitGroup
	sender := createSpanSender(nil, exitCh, &wg, log.WithField("test", nil))

	id := sender.getSqlUidMetaApiId("INSERT INTO chengji_m VALUES (%s, %s, %s)")
	t.Logf("%v", string(id))
	if len(id) != 16 {
		t.Errorf("sqlUid length = %d, want 16 (%x)", len(id), id)
	}
}
