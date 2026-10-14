## v0.7.10

> ⚠️ **SECURITY**: v0.7.9 (and earlier) ships vulnerable dependencies.
> **Upgrade to v0.7.10 as soon as possible.**

### Security
- **Upgrade vulnerable Go dependencies** (found by `govulncheck`/image scanners):
  - `golang.org/x/net` v0.9.0 → **v0.60.0** — fixes **CVE-2023-44487**
    (HTTP/2 "Rapid Reset" DoS, GO-2023-2102) plus GO-2025-3503 and
    GO-2026-6603/6611/6612/6617.
  - `google.golang.org/grpc` v1.56.3 → **v1.83.1** — fixes GO-2026-6061 and
    GO-2026-6348.
  - `google.golang.org/protobuf` v1.34.1 → **v1.36.11**.
  - The module's Go language version is raised `go 1.19` → `go 1.26.0`.
  - After the upgrade `govulncheck ./...` reports **0 reachable vulnerabilities**
    (was 7 from 2 modules). The generated gRPC code
    (`SupportPackageIsVersion7`) remains compatible — no proto regeneration
    required.

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