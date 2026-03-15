# Sniffy Agent Bridge

IPC bridge enabling external AI agents to control the Sniffy instrument app.

## Architecture

```
Sniffy App (Qt)                  AI Agent (Python / any language)
┌─────────────────┐             ┌─────────────────────────┐
│ Scope, Gen, ... │             │ LLM orchestrator        │
│       ↕         │  named pipe │   ↕                     │
│  AgentBridge    │◄═══════════►│ sniffy-mcp client       │
│  (JSON-RPC 2.0) │ "sniffy-   │ (MCP tools / direct)    │
│       ↕         │  agent"     └─────────────────────────┘
│ DeviceMediator  │
│       ↕         │
│  Serial / MCU   │
└─────────────────┘
```

The bridge runs inside the Qt app as a `QLocalServer`.  Every command the
agent sends flows through the same code path as the GUI — the user sees
every change in real time.

## Components

| Component | Path | Language |
|-----------|------|----------|
| Bridge server | `LEO_sniffy/agentbridge/` | C++ / Qt |
| Python client + MCP server | `Tools/sniffy_mcp/` | Python 3.10+ |
| Agent instructions | `Tools/sniffy_mcp/AGENT_INSTRUCTIONS.md` | Markdown |

## Quick Start

### 1. Build & run the Sniffy app

The bridge starts automatically — no configuration needed.
Look for `[AgentBridge] Listening on ...` in the debug output.

### 2. Install the Python client

```bash
cd Tools/sniffy_mcp
pip install -e .            # core client (no dependencies)
pip install -e ".[mcp]"     # with MCP server support
```

### 3. Use directly from Python

```python
from sniffy_mcp import SniffyClient

with SniffyClient() as s:
    print(s.get_status())
    s.module_start("Oscilloscope")
    data = s.wait_for_scope_data()
    print(f"Ch1: {len(data['channels'][0]['y'])} samples")
    s.module_stop("Oscilloscope")
```

### 4. Use as an MCP server (for Claude, Copilot, etc.)

```bash
python -m sniffy_mcp            # stdio transport
# or
sniffy-mcp                      # installed entry point
```

MCP client configuration (e.g. `claude_desktop_config.json`):

```json
{
  "mcpServers": {
    "sniffy": {
      "command": "python",
      "args": ["-m", "sniffy_mcp"]
    }
  }
}
```

## Wire Protocol

Length-prefixed JSON-RPC 2.0 over named pipe (`sniffy-agent`):

```
[4 bytes big-endian length][JSON payload]
```

### Methods

| Method | Params | Returns |
|--------|--------|---------|
| `ping` | — | `{pong: true}` |
| `get_status` | — | `{connected, demo_mode, device, modules[]}` |
| `list_modules` | — | `{modules[{name, prefix, status, available, active}]}` |
| `module_start` | `{module}` | `{ok: true}` |
| `module_stop` | `{module}` | `{ok: true}` |
| `module_get_config` | `{module}` | `{module, config_base64}` |
| `write_command` | `{command}` | `{ok: true}` |
| `write_command_int` | `{prefix, feature, value}` | `{ok: true}` |
| `scope_read_data` | — | `{channels[{x[], y[]}], sampling_rate, resolution}` |
| `scope_read_data_compact` | — | `{channels[{xy_base64}], ...}` |

## Adding New Modules

**No bridge code changes needed.**  The bridge auto-discovers modules from
`DeviceMediator::getModulesList()`.  A new module is automatically:

- Listed by `list_modules` and `get_status`
- Startable / stoppable via `module_start` / `module_stop`
- Configurable via `write_command` / `write_command_int`
- Config-readable via `module_get_config`

For specialized data readback (like scope traces), adding a data hook in
`AgentBridge::onRawData()` is optional — agents can always use
`write_command` to configure modules and the app GUI for visual feedback.

## Security

- Binds to **localhost only** (named pipe — no network exposure)
- 4 MB frame-size limit prevents memory exhaustion
- Max 4 concurrent connections
- No authentication (local IPC between processes of the same user)
