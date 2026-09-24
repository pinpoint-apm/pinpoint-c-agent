## v0.7.9

> ⚠️ **SECURITY**: v0.7.8 (and earlier) contains a remote denial-of-service
> vulnerability. **Upgrade to v0.7.9 as soon as possible.** See the notice below.

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
      collide in the shared cache and panic on a type assertion (namespaced SQL
      keys with a `sqlUidKeyPrefix`).
    - `CollectPStateMessage`: `totalPer[0]` could panic on an empty slice.

### Build
- Stop committing generated protobuf Go code (`pinpoint-grpc-idl-go/proto/v1/*.pb.go`).
  Regenerated at build time via `make protoc` (also in CI).

### CI
- `cpp-windows`: use Visual Studio 18 2026 (runner no longer ships VS 2022).
- `PHP-Win-2019` → `PHP-Win-2022` (windows-2022, `php/setup-php-sdk@v0.12`).
- Drop PHP 7.x from CI matrix (EOL).
- Regenerate protobuf code in the `Collector-agent` job before testing.
- Sanitize branch name in the PHP-Win artifact name (branches with `/` broke
  `Compress-Archive`).

## v0.6.4
- fix: panic: send on closed channel #658

## v0.5.3 2024.05.31
- support go install

## [v0.5.0] start time in ms
- return current time in ms