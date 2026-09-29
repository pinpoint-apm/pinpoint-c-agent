package agent

import (
	"context"
	"sync"
	"testing"

	"github.com/pinpoint-apm/pinpoint-c-agent/collector-agent/common"
)

func TestSqlUidFormat(t *testing.T) {
	m := &mockCollector{}
	addr, stop := startMockCollector(t, m)
	defer stop()

	var wg sync.WaitGroup
	config := mockConfig(addr)
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
// interface-conversion error. The cache key now includes the metadata type, so
// entries can never collide across types.
//
// Uses a mock collector so metadata registration succeeds and the cached IDs
// are not evicted by a failed send (which would make the assertions depend on
// network behavior).
func TestSqlUidKeyPrefixNoTypeConfusion(t *testing.T) {
	m := &mockCollector{}
	addr, stop := startMockCollector(t, m)
	defer stop()

	var wg sync.WaitGroup
	config := mockConfig(addr)
	sender := createSpanSender(nil, context.Background(), &wg, config, config.LogEntry)

	const shared = "SELECT * FROM t WHERE id = 1"

	// First store an int32 under the plain key (API/string metadata path).
	apiID := sender.getMetaApiId(shared, common.META_Default_api)
	if apiID <= 0 {
		t.Fatalf("getMetaApiId returned non-positive id %d", apiID)
	}

	// Then fetch a []byte for the SAME string via the SQL path. Before the fix
	// this panicked (interface conversion: interface {} is int32, not []uint8).
	// The type-aware key keeps the SQL entry on a separate key, so this must
	// succeed.
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

// TestMetaKeyIsolatesTypes verifies that the same name string used for
// different metadata types gets independent cache entries (distinct IDs), so
// one type can never shadow or corrupt another.
func TestMetaKeyIsolatesTypes(t *testing.T) {
	m := &mockCollector{}
	addr, stop := startMockCollector(t, m)
	defer stop()

	var wg sync.WaitGroup
	config := mockConfig(addr)
	sender := createSpanSender(nil, context.Background(), &wg, config, config.LogEntry)

	const shared = "same-name"

	apiID := sender.getMetaApiId(shared, common.META_Default_api)
	webID := sender.getMetaApiId(shared, common.META_Web_request_api)
	strID := sender.getMetaApiId(shared, common.META_String_api)
	invID := sender.getMetaApiId(shared, common.META_INVOCATION_API)

	ids := map[string]int32{
		"api": apiID, "web": webID, "string": strID, "invocation": invID,
	}
	seen := map[int32]string{}
	for kind, id := range ids {
		if prev, dup := seen[id]; dup {
			t.Errorf("type %s and %s share id %d — types are not isolated", kind, prev, id)
		}
		seen[id] = kind
	}

	// Re-fetch must return the same per-type IDs (stable cache).
	if got := sender.getMetaApiId(shared, common.META_Default_api); got != apiID {
		t.Errorf("api re-fetch = %d, want %d", got, apiID)
	}
	if got := sender.getMetaApiId(shared, common.META_Web_request_api); got != webID {
		t.Errorf("web re-fetch = %d, want %d", got, webID)
	}
}

// TestMetaKeyNoNulPrefixCollision reproduces the exact attack vector from the
// review (R9): an attacker-controlled API name containing a NUL byte plus the
// old SQL prefix ("\x00sqluid:SELECT 1") used to collide with the SQL string
// "SELECT 1" under the old string-key scheme, causing an int32/[]byte type
// assertion panic. With the type-aware struct key this must not panic.
func TestMetaKeyNoNulPrefixCollision(t *testing.T) {
	m := &mockCollector{}
	addr, stop := startMockCollector(t, m)
	defer stop()

	var wg sync.WaitGroup
	config := mockConfig(addr)
	sender := createSpanSender(nil, context.Background(), &wg, config, config.LogEntry)

	// Attacker-controlled API name crafted to look like the old SQL key.
	attackerAPI := "\x00sqluid:SELECT 1"
	apiID := sender.getMetaApiId(attackerAPI, common.META_Default_api)
	if apiID <= 0 {
		t.Fatalf("getMetaApiId returned non-positive id %d", apiID)
	}

	// Must NOT panic (previously: interface conversion int32 -> []uint8).
	sqlID := sender.getSqlUidMetaApiId("SELECT 1")
	if len(sqlID) != 16 {
		t.Fatalf("getSqlUidMetaApiId returned id of length %d, want 16", len(sqlID))
	}

	// The attacker's API entry must remain intact and distinct.
	if got := sender.getMetaApiId(attackerAPI, common.META_Default_api); got != apiID {
		t.Errorf("attacker API re-fetch = %d, want %d", got, apiID)
	}
}

