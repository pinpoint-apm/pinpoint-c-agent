package agent

import (
	"testing"
)

func TestSqlUidFormat(t *testing.T) {
	// test  chan
	exitCh := make(chan bool) // chan bool
	sender := createSpanSender(nil, exitCh)

	id := sender.getSqlUidMetaApiId("INSERT INTO chengji_m VALUES (%s, %s, %s)")
	t.Logf("%v", string(id))
	if len(id) == 0 {
		t.Error(id)
	}
}
