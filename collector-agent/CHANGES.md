## v0.7.9

> ⚠️ **SECURITY**: v0.7.8 (and earlier) contains a remote denial-of-service
> vulnerability. **Upgrade to v0.7.9 as soon as possible.**

### Security
- **Fix remote DoS (CVE-pending)**: an unauthenticated remote attacker could
  crash the collector-agent with a single TCP packet (type=1 `REQ_UPDATE_SPAN`),
  causing a denial of service for all traced applications.
  - `ParseDotFormatToTime` (`common/Utils.go`) accessed `ar[1]` out of bounds
    when the attacker-controlled `NP`/NginxHeader field contained no `.`
    (e.g. `"D=123"`) → index-out-of-range panic.
  - The filter-chain goroutine `handleTSpanFromBuf` had no `recover()`, so the
    panic terminated the whole process.
  - Also hardened several related panic-prone paths reachable from the same
    unauthenticated span path:
    - `createPinpointSpanEv`: annotation parsing `ann[0:iColon]` panicked on
      strings without `:` (added `iColon > 0` guard).
    - `makeSpanOrSpanChunk`: nil element in `span.Follows` (`"event":[null]`)
      caused a nil-pointer dereference (skip nil elements).
    - `idMap` type-confusion: an API-name string equal to a SQL string could
      collide in the shared cache and panic on a type assertion. The cache key
      is now a `metaKey{metaType, name}` struct, so entries of different
      metadata types (API/string/web/SQL-uid/invocation) can never collide —
      this also closes the NUL-prefix bypass (`"\u0000sqluid:..."`) that the
      earlier string-prefix scheme did not prevent.
    - `CollectPStateMessage`: `totalPer[0]` could panic on an empty slice.

### Reliability
- **Fix data race in request statistics (R6)**: `RequestCounter` fields were
  written by the span consumer goroutine and read concurrently by the
  stat/command/cleanup goroutines with no synchronization. A mutex now guards
  all statistics and snapshots (reproduced with `go test -race`).
- **Fix metadata registration not retried after a connection failure (R8)**:
  `getMetaApiId`/`getSqlUidMetaApiId` cached the assigned ID before registering
  it, so a failed dial left an unregistered ID cached forever. Entries are now
  marked *pending* until registration is confirmed and retried on next access
  (the ID stays stable).

## v0.6.4
- fix: panic: send on closed channel #658

## v0.5.3 2024.05.31
- support go install

## [v0.5.0] start time in ms
- return current time in ms