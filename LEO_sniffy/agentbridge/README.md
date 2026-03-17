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
| Python client + MCP server | `tools/sniffy_mcp/` | Python 3.10+ |
| Agent instructions | `tools/sniffy_mcp/AGENT_INSTRUCTIONS.md` | Markdown |

## Quick Start

### 1. Build & run the Sniffy app

The bridge starts automatically — no configuration needed.
Look for `[AgentBridge] Listening on ...` in the debug output.

### 2. Install the Python client

```bash
cd tools/sniffy_mcp
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

#### Core / Status

| Method | Params | Description |
|--------|--------|-------------|
| `ping` | — | Health check → `{pong: true}` |
| `get_status` | — | Connection state, device info, module list |
| `get_system_config` | — | Full HW config: device info, per-module capabilities, pins, resource conflicts |

#### Device Management

| Method | Params | Description |
|--------|--------|-------------|
| `scan_devices` | — | Probe COM ports for Sniffy boards |
| `list_devices` | — | Return last scan result (no re-scan) |
| `connect_device` | `{index}` or `{name}` | Connect by index or name substring |
| `disconnect_device` | — | Disconnect current device |

#### Module Lifecycle

| Method | Params | Description |
|--------|--------|-------------|
| `list_modules` | — | List all modules with status |
| `module_start` | `{module}` | Open & initialise a module (checks resource conflicts) |
| `module_stop` | `{module}` | Stop and close a module |
| `module_run` | `{module}` | Start signal generation (generators only) |
| `module_halt` | `{module}` | Stop signal generation without closing |
| `get_module_state` | `{module}` | Read current GUI state of a running module |
| `module_get_config` | `{module}` | Serialized config (base64) |

#### Raw Commands

| Method | Params | Description |
|--------|--------|-------------|
| `write_command` | `{command}` | Send raw ASCII protocol command |
| `write_command_int` | `{prefix, feature, value}` | Send MODULE:FEATURE + 32-bit int |

#### Oscilloscope — Data

| Method | Params | Description |
|--------|--------|-------------|
| `scope_read_data` | — | Last scope snapshot as JSON arrays |
| `scope_read_data_compact` | — | Last scope snapshot with base64-packed floats |

#### Oscilloscope — Configuration

| Method | Params | Description |
|--------|--------|-------------|
| `scope_set_timebase` | `{module, value}` | Time/div in seconds |
| `scope_set_sampling_freq` | `{module, value}` | Sampling frequency (Hz) |
| `scope_set_data_length` | `{module, value}` | Data length (samples) |
| `scope_set_channels` | `{module, channel_mask}` | Enable channels by bitmask |
| `scope_set_resolution` | `{module, bits}` | ADC resolution (8 or 12) |
| `scope_set_pretrigger` | `{module, percent}` | Pre-trigger percentage |
| `scope_set_trigger_mode` | `{module, mode}` | auto / normal / single / stop |
| `scope_set_trigger_edge` | `{module, edge}` | rising / falling |
| `scope_set_trigger_channel` | `{module, channel}` | Trigger channel (0-based) |
| `scope_set_trigger_level` | `{module, percent}` | Trigger level percentage |

#### Oscilloscope — Measurements

| Method | Params | Description |
|--------|--------|-------------|
| `scope_add_measurement` | `{module, type, channel, channel_b?}` | Add measurement (frequency, phase, rms, ...) |
| `scope_get_measurements` | `{module}` | Get current results |
| `scope_clear_measurements` | `{module}` | Clear all measurements |

#### Display Widgets

| Method | Params | Description |
|--------|--------|-------------|
| `list_displays` | `{module}` | List WidgetDisplay names in a module |
| `display_save_history` | `{module, display}` | Save display history to file |
| `display_clear_history` | `{module, display}` | Clear display history |
| `display_set_history_size` | `{module, display, size}` | Set history depth (100–1000) |

#### Sync PWM

| Method | Params | Description |
|--------|--------|-------------|
| `spwm_set_frequency` | `{module, channel, value}` | Channel frequency (Hz) |
| `spwm_set_duty` | `{module, channel, value}` | Channel duty cycle (%) |
| `spwm_set_phase` | `{module, channel, value}` | Channel phase (degrees) |
| `spwm_set_channel_enable` | `{module, channel, enabled}` | Enable/disable a channel |
| `spwm_set_step_mode` | `{module, step}` | Continuous (false) / Step (true) |
| `spwm_set_equidistant` | `{module, enabled}` | Equidistant phase distribution |
| `spwm_set_invert` | `{module, channel, enabled}` | Channel inversion |

#### Counter

| Method | Params | Description |
|--------|--------|-------------|
| `counter_set_mode` | `{module, mode}` | 0=HF, 1=LF, 2=Ratio, 3=Intervals |
| `counter_set_gate_time` | `{module, index}` | HF gate time (0=100ms .. 4=10s) |
| `counter_hf_config` | `{module, ...}` | Keys: quantity, error_mode, averaging, gate_time |
| `counter_lf_config` | `{module, ...}` | Keys: channel, quantity, multiplier, duty_cycle, sample_count, sample_channel |
| `counter_rat_set_sample_count` | `{module, value}` | Ratio counter sample count |
| `counter_int_config` | `{module, ...}` | Keys: event_sequence, edge_a, edge_b, timeout |

#### Arb / PWM Generator

| Method | Params | Description |
|--------|--------|-------------|
| `arbgen_set_frequency` | `{module, channel, value}` | Frequency (Hz) |
| `arbgen_set_shape` | `{module, channel, index}` | 0=Sine, 1=Saw, 2=Rect, 3=Arb |
| `arbgen_set_amplitude` | `{module, channel, value}` | Amplitude (V) |
| `arbgen_set_offset` | `{module, channel, value}` | Offset (V) |
| `arbgen_set_duty` | `{module, channel, value}` | Duty cycle (%) |
| `arbgen_set_phase` | `{module, channel, value}` | Phase (degrees) |
| `arbgen_set_channels` | `{module, count}` | Number of active channels |
| `arbgen_set_memory` | `{module, mode, length?}` | 0=Best fit, 1=Long, 2=Custom |
| `arbgen_set_sweep` | `{module, enable, min_freq?, max_freq?, sweep_time?}` | SW frequency sweep on CH1 |
| `arbgen_set_freq_sync` | `{module, channel, enabled}` | Sync frequency with CH1 |
| `arbgen_set_pwm_frequency` | `{module, channel, value}` | PWM carrier frequency |
| `arbgen_set_pwm_freq_sync` | `{module, channel, enabled}` | PWM freq sync with CH1 |

#### Pattern Generator

| Method | Params | Description |
|--------|--------|-------------|
| `patgen_set_pattern` | `{module, index}` | Select pattern type (0–14) |
| `patgen_set_frequency` | `{module, value}` | Frequency of current pattern |
| `patgen_set_channels` | `{module, value}` | Channel count / data length |
| `patgen_reset` | `{module}` | Reset pattern to defaults |

#### Voltage Source

| Method | Params | Description |
|--------|--------|-------------|
| `vout_set_voltage` | `{module, channel, voltage}` | Set DAC output voltage |

#### Voltmeter

| Method | Params | Description |
|--------|--------|-------------|
| `voltmeter_set_mode` | `{module, mode}` | 0=Normal, 1=Fast |
| `voltmeter_set_averaging` | `{module, value}` | Averaging samples (1–64) |
| `voltmeter_set_calc_mode` | `{module, mode}` | 0=Min/Max, 1=Ripple, 2=None |
| `voltmeter_set_channels` | `{module, channel_mask}` | Enable channels by bitmask |
| `voltmeter_get_readings` | `{module}` | Live readings (voltage, min, max, ripple, freq) |
| `voltmeter_set_datalog_file` | `{module, path}` | Set data log file path |
| `voltmeter_start_datalog` | `{module}` | Start data logging |
| `voltmeter_stop_datalog` | `{module}` | Stop data logging |

## Adding New Modules

The bridge auto-discovers modules from `DeviceMediator::getModulesList()`.
A new module is automatically:

- Listed by `list_modules`, `get_status`, and `get_system_config`
- Startable / stoppable via `module_start` / `module_stop`
- State-readable via `get_module_state`
- Configurable via `write_command` / `write_command_int`
- Config-readable via `module_get_config`

For GUI-synced control of module-specific widgets (dials, buttons, etc.),
add dedicated handlers in `AgentBridge::registerHandlers()`.  For scope-like
data streaming, add a data hook in `AgentBridge::onRawData()`.

## Security

- Binds to **localhost only** (named pipe — no network exposure)
- 4 MB frame-size limit prevents memory exhaustion
- Max 4 concurrent connections
- No authentication (local IPC between processes of the same user)
