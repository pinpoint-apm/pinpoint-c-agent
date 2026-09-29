# Pinpoint C Agent AI Instructions

Welcome! This repository contains the Pinpoint PHP and Python agents, a shared C++ core, and a Go-based collector agent.

## Project Architecture

The project follows a tiered architecture:
1.  **Language Agents (PHP/Python)**: Small, high-performance extensions (C++) or SDKs that hook into the runtime to record spans.
2.  **Common C++ Core ([common/](common/))**: Shared logic for span management and serialization used by both agents.
3.  **Collector Agent ([collector-agent/](collector-agent/))**: A Go-based bridge that receives data from agents (via TCP/UDS) and forwards it to the Pinpoint Collector via gRPC.

**Data Flow**: `App` -> `PHP/Python Agent (C++)` -> `Collector Agent (Go)` -> `Pinpoint Collector (Java)`.

## Build & Test Commands

### PHP Extension
- **Build**: `phpize && ./configure && make`
- **Install**: `sudo make install`
- **Test**: `make test` (runs `.phpt` tests in [tests/](tests/))

### Python Agent
- **Build**: `pip install .`
- **Test**: `python -m unittest discover src/PY/test`

### Go Collector Agent
- **Build**: `cd collector-agent && make server`
- **Test**: `cd collector-agent && make test`

### Common C++ Core
- **Build**: `cd common && mkdir build && cd build && cmake .. && make`
- **Test**: `CTEST_OUTPUT_ON_FAILURE=1 make test` (requires `-DWITH_TEST_CASE=ON` in cmake)

## Important Conventions & Pitfalls

- **Shared Core**: Any changes to [common/](common/) impact **both** PHP and Python agents. Ensure both are tested/validated.
- **PHP AOP Cache**: When editing PHP AOP plugins, you may need to clear the cache: `rm /tmp/.cache/.__class_index.php`.
- **Go Collector Protoc**: Compiling protobufs for the Go collector requires `protoc` and `protoc-gen-go` to be installed and in the PATH. See [collector-agent/makefile](collector-agent/makefile) for the `protoc` command.
- **Build Stability**: Recent changes have removed "version passing" features to improve build stability. Avoid re-introducing complex versioning logic into the build scripts without extensive testing.
- **Documentation**: Extensive documentation is available in the [DOC/](DOC/) directory for each component. Refer to them before making architectural changes.
- **Environment Variables**: The Collector Agent is configured primarily via environment variables (see [collector-agent/env.list](collector-agent/env.list)).

## Key Directories

- [common/](common/): The "heart" of the C-agent logic.
- [collector-agent/](collector-agent/): Bridge to the Pinpoint backend.
- [plugins/](plugins/): Language-specific framework support (Laravel, Django, etc.).
- [testapps/](testapps/): Docker-based integration tests. Use these for full end-to-end verification.

## Knowledge Base Auto-Update (Self-Learning)

The AI maintains a repository-scoped knowledge base under `/memories/repo/` (e.g., `build-test-commands.md`, `php-agent.md`, `python-agent.md`, etc.). This knowledge base must be kept up-to-date as the project evolves and as mistakes are corrected.

**When to update the knowledge base automatically:**

1. **Human Correction**: When the user corrects a mistake, misconception, or outdated assumption made by the AI (e.g., "No, the Python agent doesn't use X, it uses Y", "That command is wrong, use this instead"), the AI must:
   - Acknowledge the correction.
   - Update the relevant `/memories/repo/*.md` file(s) to reflect the corrected information.
   - If the correction reveals a new topic not covered by an existing file, create a new memory file or append to the most relevant one.

2. **Self-Correction**: When the AI discovers its own error during a task (e.g., a build command fails because the documented steps were wrong, a file path was incorrect, an assumption about the codebase was invalid), the AI must:
   - Fix the immediate issue.
   - Update the knowledge base so the same mistake is not repeated in future sessions.

3. **New Discoveries**: When the AI learns a new, non-obvious fact about the codebase (e.g., a hidden dependency, a workaround, an environment-specific quirk, a build order requirement), record it in the appropriate memory file.

**How to update:**

- Use the `memory` tool with `command: "str_replace"` to update existing entries, or `command: "create"` to add new files.
- Keep entries **short and concise** — use bullet points or single-line facts, not lengthy prose.
- Organize by topic. Prefer updating an existing file over creating a new one.
- Remove or correct entries that are proven wrong or outdated.
- Do **not** record trivial or already-documented information (e.g., things clearly stated in `AGENTS.md` or README files).

**Examples of corrections worth recording:**
- A build command that differs from what's documented.
- A test that requires a specific environment variable or service to be running.
- A code pattern or convention that is not obvious from reading the code.
- A pitfall that caused a build failure or test failure.
- An incorrect assumption about which component owns a particular feature.
