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
	if len(id) != 16 {
		t.Errorf("sqlUid length = %d, want 16 (%x)", len(id), id)
	}
}

// Regression test for the type-confusion fix: getMetaApiId stores int32 in the
// shared idMap, while getSqlUidMetaApiId stores []byte. If the same string is
// used for both an API name and a SQL string, the two accessors previously
// collided on the same map key and the second type assertion panicked with an
// interface-conversion error. The sqlUidKeyPrefix namespaces SQL entries so
// this can no longer happen.
func TestSqlUidKeyPrefixNoTypeConfusion(t *testing.T) {
	var wg sync.WaitGroup
	config := common.CreateTestConfig()
	sender := createSpanSender(nil, context.Background(), &wg, config, config.LogEntry)

	const shared = "SELECT * FROM t WHERE id = 1"

	// First store an int32 under the plain key (API/string metadata path).
	apiID := sender.getMetaApiId(shared, common.META_Default_api)
	if apiID <= 0 {
		t.Fatalf("getMetaApiId returned non-positive id %d", apiID)
	}

	// Then fetch a []byte for the SAME string via the SQL path. Before the fix
	// this panicked (interface conversion: interface {} is int32, not []uint8).
	// The prefix keeps the SQL entry on a separate key, so this must succeed.
	sqlID := sender.getSqlUidMetaApiId(shared)
	if len(sqlID) != 16 {
		t.Fatalf("getSqlUidMetaApiId returned id of length %d, want 16", len(sqlID))
	}

	// Both cached entries must remain independently accessible and correct.
	if got := sender.getMetaApiId(shared, common.META_Default_api); got != apiID {
		t.Errorf("getMetaApiId after SQL insert = %d, want %d", got, apiID)
	}
	if got := sender.getSqlUidMetaApiId(shared); len(got) != 16 {
		t.Errorf("getSqlUidMetaApiId after re-fetch returned id of length %d, want 16", len(got))
	}
}

