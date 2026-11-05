# Common Change History

## 0.6.1 2026-11-03

- harden stream framing: reject oversized frame lengths, use `memmove` for
  buffer shifts, and handle incomplete messages gracefully
- validate socket descriptors against `FD_SETSIZE` on POSIX before `select`
- clear `context_map_` on `TraceNode::Reset` to avoid stale node reuse state
- compare collector-agent hello version numerically (optional `v` prefix,
  dotted parts as numbers) so `0.7.1`, `v0.7`, `v0.10.x` sort correctly (#758)

## 0.6.0 2024-10-10

- refactor span json protocol
- add async api
- data structure change: multiple map to list
- add `pinpoint_get_depth`

## 0.5.3 2024-07-30

- support unittest in macos

## 0.5.2 2024-07-26

- fix `tick++`

## 0.5.1 2024-05-07

- fix logging bug
- add EXP_V2 protocol 2024-04-30

## 0.5.0 2024-03-06

- refactor common library
- replace start_time with ms format
- remove dependence of shm

## 0.6.0 2024-10-10

- refactor span json protocol
- add async api
- data structure change: multiple map to list
- add `pinpoint_get_depth`

## 0.5.3 2024-07-30

- support unittest in macos

## 0.5.2 2024-07-26

- fix `tick++`

## 0.5.1 2024-05-07

- fix logging bug
- add EXP_V2 protocol 2024-04-30

## 0.5.0 2024-03-06

- refactor common library
- replace start_time with ms format
- remove dependence of shm

</details>


<details>
<summary id="collector-agent">Collector-agent — latest 0.7.10</summary>
