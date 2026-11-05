# Changes

Release history by product. Use the table to locate a release, then expand its agent section for the full history.

<!-- BEGIN GENERATED RELEASE SUMMARY -->
| Agent | Latest version | Security update (upgrade recommended) | Release history |
| --- | --- | --- | --- |
| PHP agent | 0.6.2 | — | [View history](#php-agent) |
| Python agent | 1.4.2 | — | [View history](#python-agent) |
| Common library | 0.6.1 | — | [View history](#common-library) |
| Collector-agent | 0.7.10 | ⚠️ Upgrade to v0.7.10 as soon as possible ([details](#collector-agent)) | [View history](#collector-agent) |
<!-- END GENERATED RELEASE SUMMARY -->

<details>
<summary id="php-agent">PHP agent — latest 0.6.2</summary>

### PHP (0.6.2, 2026-11-03)

- preserve interceptor exception and bailout semantics (#764)
- fix inherited internal method interceptor resolution

### PHP (0.5.3, 2024-07-31)

- build on win32
- build on macos
- support SendSpanTimeOutMs=-1

### PHP (0.5.2, 2024-05-30)

- add pinpoint_get_caller_arg

### PHP (0.5.1, 2024-05-29)

- add _pinpoint_join_cut and test case
- rename api naming
- add pinpoint_get_this api and test case
- remove php5 support

### PHP (0.5.0)

- support common library v0.5.0

### PHP (0.5.3, 2024-07-31)

- build on win32
- build on macos
- support SendSpanTimeOutMs=-1

### PHP (0.5.2, 2024-05-30)

- add pinpoint_get_caller_arg

### PHP (0.5.1, 2024-05-29)

- add _pinpoint_join_cut and test case
- rename api naming
- add pinpoint_get_this api and test case
- remove php5 support

### PHP (0.5.0)

- support common library v0.5.0

</details>


<details>
<summary id="python-agent">Python agent — latest 1.4.2</summary>

### Python (1.4.2, 2026-11-03)

- preserve native API errors and trace state (#763)

</details>


<details>
<summary id="common-library">Common library — latest 0.6.1</summary>

### Common (0.6.1, 2026-11-03)

- harden stream framing: reject oversized frame lengths, use `memmove` for
  buffer shifts, and handle incomplete messages gracefully
- validate socket descriptors against `FD_SETSIZE` on POSIX before `select`
- clear `context_map_` on `TraceNode::Reset` to avoid stale node reuse state
- compare collector-agent hello version numerically (optional `v` prefix,
  dotted parts as numbers) so `0.7.1`, `v0.7`, `v0.10.x` sort correctly (#758)

### Common (0.6.0, 2024-10-10)

- refactor span json protocol
- add async api
- data structure change: multiple map to list
- add `pinpoint_get_depth`

### Common (0.5.3, 2024-07-30)

- support unittest in macos

### Common (0.5.2, 2024-07-26)

- fix `tick++`

### Common (0.5.1, 2024-05-07)

- fix logging bug
- add EXP_V2 protocol 2024-04-30

### Common (0.5.0, 2024-03-06)

- refactor common library
- replace start_time with ms format
- remove dependence of shm

### Common (0.6.0, 2024-10-10)

- refactor span json protocol
- add async api
- data structure change: multiple map to list
- add `pinpoint_get_depth`

### Common (0.5.3, 2024-07-30)

- support unittest in macos

### Common (0.5.2, 2024-07-26)

- fix `tick++`

### Common (0.5.1, 2024-05-07)

- fix logging bug
- add EXP_V2 protocol 2024-04-30

### Common (0.5.0, 2024-03-06)

- refactor common library
- replace start_time with ms format
- remove dependence of shm

</details>


<details>
<summary id="collector-agent">Collector-agent — latest 0.7.10</summary>

### Collector-agent (0.7.10, 2026-11-03)

> ⚠️ **SECURITY**: v0.7.9 (and earlier) ships vulnerable dependencies.
> **Upgrade to v0.7.10 as soon as possible.**

#### Security
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

### Collector-agent (0.7.9, 2026-11-03)

> ⚠️ **SECURITY**: v0.7.8 (and earlier) contains a remote denial-of-service
> vulnerability. **Upgrade to v0.7.9 as soon as possible.**

#### Security
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

#### Reliability
- **Fix data race in request statistics (R6)**: `RequestCounter` fields were
  written by the span consumer goroutine and read concurrently by the
  stat/command/cleanup goroutines with no synchronization. A mutex now guards
  all statistics and snapshots (reproduced with `go test -race`).
- **Fix metadata registration not retried after a connection failure (R8)**:
  `getMetaApiId`/`getSqlUidMetaApiId` cached the assigned ID before registering
  it, so a failed dial left an unregistered ID cached forever. Entries are now
  marked *pending* until registration is confirmed and retried on next access
  (the ID stays stable).

### Collector-agent (0.6.4, 2026-11-03)

- fix: panic: send on closed channel #658

### Collector-agent (0.5.3, 2026-11-03)

- support go install

### Collector-agent (v0.5.0)

- return current time in ms

</details>
