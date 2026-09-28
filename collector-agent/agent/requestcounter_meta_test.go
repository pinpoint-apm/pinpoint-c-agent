package agent

import (
	"context"
	"sync"
	"testing"
	"time"

	"github.com/pinpoint-apm/pinpoint-c-agent/collector-agent/common"
)

// TestRequestCounterRace reproduces the R6 data race: the span consumer
// goroutine writes the statistics (Interceptor) while the stat/command/cleanup
// goroutines read them (GetMaxAvg/GetReqTimeProfiler/GetLastBusyTime). Run with
// `go test -race`; before the mutex was added this failed with a data race.
func TestRequestCounterRace(t *testing.T) {
	config := common.CreateTestConfig()
	config.StatInterval = 5 * time.Second
	rc := createRequestCounter(config)

	var wg sync.WaitGroup
	wg.Add(3)

	// Writer: span consumer.
	go func() {
		defer wg.Done()
		for i := 0; i < 20000; i++ {
			rc.Interceptor(&TSpan{ElapsedTime: int32(i % 5000)})
		}
	}()

	// Reader: stat collection.
	go func() {
		defer wg.Done()
		for i := 0; i < 20000; i++ {
			rc.GetMaxAvg()
			rc.GetReqTimeProfiler()
		}
	}()

	// Reader: router cleanup.
	go func() {
		defer wg.Done()
		for i := 0; i < 20000; i++ {
			rc.GetLastBusyTime()
		}
	}()

	wg.Wait()
}

// TestRequestCounterConcurrentAccess exercises the same accessors under -race
// with a short StatInterval so the reset branch is also hit concurrently.
func TestRequestCounterConcurrentAccess(t *testing.T) {
	config := common.CreateTestConfig()
	config.StatInterval = 1 * time.Second
	rc := createRequestCounter(config)

	var wg sync.WaitGroup
	stop := make(chan struct{})

	for w := 0; w < 4; w++ {
		wg.Add(1)
		go func() {
			defer wg.Done()
			for {
				select {
				case <-stop:
					return
				default:
					rc.Interceptor(&TSpan{ElapsedTime: 100})
				}
			}
		}()
	}
	for r := 0; r < 4; r++ {
		wg.Add(1)
		go func() {
			defer wg.Done()
			for {
				select {
				case <-stop:
					return
				default:
					rc.GetMaxAvg()
					rc.GetReqTimeProfiler()
					rc.GetLastBusyTime()
				}
			}
		}()
	}

	time.Sleep(300 * time.Millisecond)
	close(stop)
	wg.Wait()
}

// TestMetaRetryAfterDialFailure verifies R8: when the metadata backend is
// unreachable, the assigned ID stays cached but marked pending, and a later
// access retries registration once the backend is reachable again (instead of
// permanently treating the unregistered ID as valid).
func TestMetaRetryAfterDialFailure(t *testing.T) {
	// Point at a closed port so the first registration attempt fails.
	config := common.CreateTestConfig()
	config.User.AgentAddress = "127.0.0.1:1" // nothing listening
	config.GrpcConTextTimeOut = 200 * time.Millisecond
	config.MetaDataTimeWait = 200 * time.Millisecond

	var wg sync.WaitGroup
	sender := createSpanSender(nil, context.Background(), &wg, config, config.LogEntry)

	const name = "retry-me"
	key := metaKey{metaType: common.META_Default_api, name: name}

	id1 := sender.getMetaApiId(name, common.META_Default_api)
	if id1 <= 0 {
		t.Fatalf("getMetaApiId returned non-positive id %d", id1)
	}

	// The entry must be present but still pending (registration failed).
	sender.idMapMutex.Lock()
	_, cached := sender.idMap[key]
	pending := sender.pendingMeta[key]
	sender.idMapMutex.Unlock()
	if !cached {
		t.Fatal("expected ID to be cached after failed registration")
	}
	if !pending {
		t.Fatal("expected entry to remain pending after failed registration")
	}

	// Now bring up a mock collector and re-point the sender at it.
	m := &mockCollector{}
	addr, stop := startMockCollector(t, m)
	defer stop()
	sender.config.User.AgentAddress = addr
	sender.config.GrpcConTextTimeOut = 2 * time.Second

	// Re-access: must retry registration and, on success, clear pending.
	id2 := sender.getMetaApiId(name, common.META_Default_api)
	if id2 != id1 {
		t.Errorf("retry changed the ID: got %d, want %d (ID must be stable)", id2, id1)
	}

	deadline := time.Now().Add(2 * time.Second)
	for {
		sender.idMapMutex.Lock()
		stillPending := sender.pendingMeta[key]
		sender.idMapMutex.Unlock()
		if !stillPending {
			break
		}
		if time.Now().After(deadline) {
			t.Fatal("entry still pending after successful retry — registration not confirmed")
		}
		time.Sleep(20 * time.Millisecond)
	}

	if m.metaReceived.Load() == 0 {
		t.Error("expected the mock collector to receive a metadata registration on retry")
	}
}
