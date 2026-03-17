# Sniffy Agent Instructions

You are controlling a **Sniffy** hardware test instrument — a PC application
connected to an STM32 Nucleo MCU via USB-serial.  The MCU provides:
oscilloscope, arbitrary generator, PWM generator, pattern generator,
counter, voltage source, and voltmeter modules.

You interact with the hardware through the **AgentBridge** — an IPC server
running inside the Sniffy desktop app.  Every action you take is mirrored
in the GUI so the user can observe in real time.

---

## Setup & Running the CLI

The automation package lives at `LEO_sniffy/tools/sniffy_mcp/`.

> **Critical**: The CLI must be invoked from the `LEO_sniffy/tools/`
> directory (the parent of the `sniffy_mcp` package), **not** from inside
> `sniffy_mcp/` itself.

```bash
cd LEO_sniffy/tools
python -m sniffy_mcp call <method> [args...]
```

The CLI uses **SniffyClient method names** (e.g. `get_status`,
`module_start`), NOT MCP tool names (e.g. `sniffy_get_status`).
Arguments are auto-coerced: integers and floats are detected
automatically; strings are passed as-is.

### CLI examples

```bash
python -m sniffy_mcp call get_status
python -m sniffy_mcp call scan_devices
python -m sniffy_mcp call connect_device 0              # by index
python -m sniffy_mcp call connect_device "F303RE"       # name substring — passed as string
python -m sniffy_mcp call module_start "Sync PWM"
python -m sniffy_mcp call counter_set_mode "Counter" 3
python -m sniffy_mcp call spwm_set_frequency "Sync PWM" 0 5000
python -m sniffy_mcp call spwm_set_step_mode "Sync PWM" 1
python -m sniffy_mcp call spwm_set_invert "Sync PWM" 0 1
python -m sniffy_mcp call counter_hf_config Counter quantity=1 gate_time=2
python -m sniffy_mcp call list_displays Counter
python -m sniffy_mcp call display_save_history Counter LowFreqCh2Counter
```

> The `connect_device` method accepts **either** an integer index as its
> first positional argument OR a string name substring.  The CLI auto-
> coerces, so `0` becomes `int` and `"F303RE"` stays `str`.
>
> Methods that take **keyword arguments** (e.g. `counter_hf_config`,
> `counter_lf_config`, `counter_int_config`) use `key=value` pairs on the
> CLI.  Positional and keyword args can be mixed:
> `python -m sniffy_mcp call counter_lf_config Counter channel=0 duty_cycle=1`

### Python script

```python
from sniffy_mcp import SniffyClient
with SniffyClient() as s:
    print(s.get_status())
```

### MCP server (for MCP-compatible hosts)

```bash
cd LEO_sniffy/tools
pip install -e ".[mcp]"
python -m sniffy_mcp          # starts MCP stdio server
```

MCP tools are prefixed with `sniffy_` (e.g., `sniffy_get_status`).

---

## Startup Checklist

**Always follow this sequence at the beginning of every session:**

1. **`get_status`** — verify the app is reachable
2. If `connected: false`:
   a. **`scan_devices`** — probe COM ports for Sniffy boards
   b. **`connect_device`** — pick the right board by index or name
   c. **Wait for device ready** — the device needs time to enumerate
      modules and restore configuration after connection.
3. If `connected: true` — proceed to module operations

> **Important**: After `connect_device`, the MCU exchanges capability
> and configuration packets with the PC app.  If you call `module_start`
> immediately, the module may not be ready yet.  Therefore, use one
> of the following two methods:
>
> **Robust approach (recommended for automated test systems)** — use the
> Python helper `wait_for_device()` which polls `get_status` until
> `connected == true`:
> ```python
> s.connect_device(name="F303RE")
> if not s.wait_for_device(timeout=15, poll=0.3):
>     raise RuntimeError("Device did not become ready")
> s.module_start("Oscilloscope")
> ```
> Or poll manually from the CLI:
> ```bash
> python -m sniffy_mcp call connect_device 0
> # poll until connected
> python -m sniffy_mcp call get_status   # repeat until connected=true
> ```
>
> If only one board is attached and the app just launched, it may
> auto-connect.  Check `connected` and `device` fields first.
>
> **Simple approach** — in shell scripts use `Start-Sleep -Seconds 3`
> (PowerShell) or `sleep 3` (bash) between connection and module start.
---

## Device Management

| Tool (MCP) / Method (CLI) | Purpose |
|--------------------------|---------|
| `sniffy_scan_devices` / `scan_devices` | Probe all COM ports — returns list of boards |
| `sniffy_list_devices` / `list_devices` | Return cached list from last scan (no re-scan) |
| `sniffy_connect_device` / `connect_device` | Connect by index or name substring |
| `sniffy_disconnect_device` / `disconnect_device` | Close current connection |

### `connect_device` parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `index` | int | Device index from `scan_devices` result |
| `name` | string | Substring match (e.g. `"F303RE"`, `"G474RE"`) |

Provide **one** of `index` or `name`.  In CLI mode, a single positional
argument is used: integer → index, string → name.

### Supported boards

| Board | Firmware name |
|-------|---------------|
| Nucleo-F303RE | `Nucleo-F303RE` |
| Nucleo-F446RE | `Nucleo-F446RE` |
| Nucleo-G474RE | `Nucleo-G474RE` |
| Nucleo-L476RG | `Nucleo-L476RG` |

---

## Module Lifecycle

Each module follows: **stopped → active → generating → stopped**.

| Action | Tool / Method | Notes |
|--------|--------------|-------|
| Open & configure HW | `module_start` | Initialises hardware, returns `state` with session-restored config |
| Start signal output | `module_run` | Clicks the internal Start/Generate button (Sync PWM, ArbGen, PatGen) |
| Stop signal output | `module_halt` | Stops generation without closing the module |
| Close & release HW | `module_stop` | Releases hardware resources |
| Check state | `list_modules` | `status` field per module |
| Read GUI state | `get_module_state` | Current mode, dials, buttons — use any time |

> **`module_start` ≠ signal generation.** For Sync PWM, Arbitrary/PWM
> Generator, and Pattern Generator, `module_start` only opens the module
> and initialises hardware.  You **must** call `module_run` afterwards to
> actually start outputting a signal.  Counter and Scope start measuring
> automatically on `module_start` — they do not need `module_run`.

> **Correct ordering:** 1) `module_start` → 2) set parameters → 3) `module_run`.
> Always configure parameters **before** calling `module_run`.
> Changing parameters on a module that isn't started has no effect.

### Uploading transition (Generators - ArbGen/PatGen/ModPwmGen)

When `module_run` is called on the Arbitrary Generator, PWM Generator, or
Pattern Generator, the signal data must first be uploaded to the MCU.  The
response includes a **`generation_status`** field:

| `generation_status` | Meaning |
|--------------------|---------|
| `"uploading"` | Data is being uploaded to the MCU — not generating yet |
| `"running"` | Upload complete, signal is being generated |
| `"stopped"` | Not generating |

`get_module_state` also returns `generation_status` for these modules.
**If you need to run a generator for a timed duration**, poll
`get_module_state` until `generation_status` becomes `"running"` before
starting your timer.  Otherwise you may call `module_halt` while data is
still uploading and the signal never actually ran.

Sync PWM starts instantly — `generation_status` is always `"running"`
immediately after `module_run`.

### Resource conflicts

Modules that share hardware resources **cannot run simultaneously**.
`module_start` **enforces** this — it will return an error with a list of
conflicting module names if any are already active.  Stop the conflicting
module(s) first.

Use `get_system_config` to discover which modules conflict **before**
trying to start them.

> **Session persistence**: The app restores the last-used configuration
> (mode, dials, tabs) from a session file on launch.  **Always check the
> `state` returned by `module_start`** (or call `get_module_state`) to
> learn which mode/tab is active — do not assume defaults.  Then use the
> appropriate setter to change any setting that differs from what you need.

---

## System Configuration

Call `get_system_config` (or `sniffy_get_system_config` via MCP) after
connecting to discover the full hardware configuration.  This returns:

- **`device`** — MCU family, core clock, firmware version, buffer length,
  UART speed, USB support
- **`modules`** — array of available modules, each with:
  - `name`, `status`
  - Module-specific capabilities: channel count, max sampling rates,
    memory sizes, pin names, voltage references, DAC resolution, etc.
- **`conflicts`** — array of `[moduleA, moduleB]` pairs that **cannot**
  run simultaneously (they share hardware resources)

### Example response (abridged)

```json
{
  "device": {
    "name": "Nucleo-F303RE",
    "mcu": "STM32F303xE",
    "core_clock": 72000000,
    "fw_version": "2.0.0"
  },
  "modules": [
    {"name": "Oscilloscope", "channels": 2, "max_sampling_rate_12b": 5000000, "pins": ["PA0", "PA1"]},
    {"name": "Arbitrary generator", "channels": 1, "max_sampling_rate": 1500000, "pins": ["PA4"]},
    {"name": "PWM generator", "channels": 4, "timer_clock": 72000000, "pins": ["PA8","PA9","PA10","PA11"]}
  ],
  "conflicts": [
    ["Oscilloscope", "Voltmeter"],
    ["Arbitrary generator", "Voltage source"]
  ]
}
```

> **Best practice**: Call `get_system_config` once after connecting, then
> use the `channels` field to know how many channels you can enable, and
> check `conflicts` before starting multiple modules.

---

## Module Names (exact strings)

| Module | Name string |
|--------|-------------|
| Device info | `Device` |
| Oscilloscope | `Oscilloscope` |
| Counter | `Counter` |
| Voltmeter | `Voltmeter` |
| Sync PWM | `Sync PWM` |
| Arb generator (DAC) | `Arbitrary generator` |
| PWM generator | `PWM generator` |
| Pattern generator | `Pattern generator` |
| Voltage source | `Voltage source` |

---

## Protocol Command Reference

Commands use 4-char module prefixes and colon-separated fields.
Use `sniffy_write_command` for ASCII commands, `sniffy_write_command_int`
for commands with 32-bit integer parameters.

### Scope (prefix `OSCP`)

| Command | Purpose | Example |
|---------|---------|---------|
| `OSCP:ADEF` | Set default ADC channels | |
| `OSCP:DATA:12B_` | Resolution (12B_, 10B_, 8B__, 6B__) | |
| `OSCP:LENG` + int | Sample count | `write_command_int("OSCP", "LENG", 1200)` |
| `OSCP:FREQ` + int | Sampling frequency Hz | `write_command_int("OSCP", "FREQ", 100000)` |
| `OSCP:LEVL` + int | Trigger level (0-65535) | `write_command_int("OSCP", "LEVL", 32767)` |
| `OSCP:PRET` + int | Pre-trigger (0-65535) | `write_command_int("OSCP", "PRET", 32767)` |
| `OSCP:EDGE:RISE` | Rising edge trigger | Also `FALL` |
| `OSCP:CHAN:2CH_` | Number of channels | `1CH_`, `2CH_`, `3CH_`, `4CH_` |
| `OSCP:TRCH:1CH_` | Trigger channel | |
| `OSCP:TRIG:AUTO` | Trigger mode | `NORM`, `AUTO`, `SING`, `F_A_` |
| `OSCP:STRT` | Start acquisition | |
| `OSCP:STOP` | Stop acquisition | |
| `OSCP:NEXT` | Request next acquisition | |

**Reading scope data**: after starting acquisition, call `sniffy_scope_read_data`.
The data arrives asynchronously from the MCU.  Poll or use `wait_for_scope_data()`
in Python.  Returns voltage (V) vs time (s) per channel.

### Arbitrary Generator (prefix `GENS`)

| Command | Purpose |
|---------|---------|
| `GENS:MODE:DAC_` | DAC output mode |
| `GENS:LCH1` + int | CH1 data length |
| `GENS:LCH2` + int | CH2 data length |
| `GENS:CHAN:2CH_` | Number of output channels |
| `GENS:FREQ` + int | Frequency (encoded: `(freq_hz << 8) \| channel_1based`) |
| `GENS:STRT` | Start output |
| `GENS:STOP` | Stop output |
| `GENS:DINI` | De-init |

> Prefer using `sniffy_module_start("Arbitrary generator")` for standard
> operation — the app handles the full data upload sequence.

### PWM Generator (prefix `GENP`)

Same command set as arbitrary generator but with PWM mode.

### Counter (prefix `CNT_`)

| Command | Purpose |
|---------|---------|
| `CNT_:MODE:ETR_` | High frequency mode |
| `CNT_:MODE:IC__` | Low frequency mode |
| `CNT_:MODE:RAT_` | Ratio mode |
| `CNT_:MODE:TI__` | Time interval mode |
| `CNT_:STRT` | Start counting |
| `CNT_:STOP` | Stop counting |
| `CNT_:PAUS` | Hold/pause |
| `CNT_:UPAU` | Resume |

### Voltage Source (prefix `VOUT`)

| Command | Purpose |
|---------|---------|
| `VOUT:STRT` | Start output |
| `VOUT:STOP` | Stop output |
| `VOUT:DAC_` + binary | Set DAC values |

> Use `sniffy_module_start("Voltage source")` then the GUI dial for
> interactive control, or send raw DAC commands for scripted control.

### Sync PWM (prefix `SPWM`)

| Command | Purpose |
|---------|---------|
| `SPWM:SCOM:STRT` | Start all channels |
| `SPWM:SCOM:STOP` | Stop |
| `SPWM:SPCF` + binary | Set frequency config |
| `SPWM:SPDP` + binary | Set duty/phase config |

---

## Display History Management

Modules with measurement displays (Counter, Voltmeter) have named
`WidgetDisplay` instances that record history.  Use `sniffy_list_displays`
to discover the names.

### Counter display names

| Display | Name string |
|---------|-------------|
| High Frequency | `HighFreqCounter` |
| Low Freq CH1 | `LowFreqCh1Counter` |
| Low Freq CH2 | `LowFreqCh2Counter` |
| Ratio | `RatioCounter` |
| Intervals | `IntervalsCounter` |

### History tools

| Tool | Purpose |
|------|---------|
| `sniffy_list_displays` | List display names inside a module |
| `sniffy_display_save_history` | Save history to `log_<name>.txt` |
| `sniffy_display_clear_history` | Clear history (chart + list) |
| `sniffy_display_set_history_size` | Set sample count (100–1000, same as the dial) |

---

## GUI-Synced High-Level Controls

These tools update **both** the GUI widgets **and** send MCU commands,
keeping the desktop app perfectly in sync with agent actions.
**Prefer these over raw `write_command`** whenever available.

> **CLI naming rule**: The tables below list **MCP tool names** (prefixed
> with `sniffy_`).  When using the CLI (`python -m sniffy_mcp call ...`),
> **strip the `sniffy_` prefix**.  For example:
> - MCP tool: `sniffy_counter_set_mode` → CLI: `counter_set_mode`
> - MCP tool: `sniffy_list_displays` → CLI: `list_displays`
> - MCP tool: `sniffy_display_save_history` → CLI: `display_save_history`

### Sync PWM

| Tool | Parameters | Notes |
|------|-----------|-------|
| `sniffy_spwm_set_frequency` | `module`, `channel` (0-3), `value` (Hz) | Updates dial + MCU |
| `sniffy_spwm_set_duty` | `module`, `channel` (0-3), `value` (%) | Duty cycle 0-100 |
| `sniffy_spwm_set_phase` | `module`, `channel` (0-3), `value` (°) | Phase 0-360 degrees |
| `sniffy_spwm_set_channel_enable` | `module`, `channel` (0-3), `enabled` (bool) | ON/OFF toggle |
| `sniffy_spwm_set_step_mode` | `module`, `step` (bool) | false=Continuous, true=Step |
| `sniffy_spwm_set_equidistant` | `module`, `enabled` (bool) | Equidistant phase distribution (syncs CH1-3 to CH0) |
| `sniffy_spwm_set_invert` | `module`, `channel` (0-3), `enabled` (bool) | Invert waveform per channel |

**State readback** (`sniffy_get_module_state`) returns:
- `step_mode` (bool), `equidistant` (bool)
- Per channel: `enabled`, `inverted`, `frequency`, `duty`, `phase`

**Workflow example — Set CH0 to 5 kHz 50% inverted, equidistant step mode:**
```
1. sniffy_module_start("Sync PWM")
2. sniffy_spwm_set_frequency(module="Sync PWM", channel=0, value=5000)
3. sniffy_spwm_set_duty(module="Sync PWM", channel=0, value=50)
4. sniffy_spwm_set_invert(module="Sync PWM", channel=0, enabled=true)
5. sniffy_spwm_set_step_mode(module="Sync PWM", step=true)
6. sniffy_spwm_set_equidistant(module="Sync PWM", enabled=true)
7. sniffy_module_run(module="Sync PWM")       ← starts signal output
8. sniffy_get_module_state(module="Sync PWM")  → verify all settings
```

### Counter

| Tool | Parameters | Notes |
|------|-----------|-------|
| `sniffy_counter_set_mode` | `module`, `mode` (0-3) | 0=HF, 1=LF, 2=Ratio, 3=Intervals |
| `sniffy_counter_set_gate_time` | `module`, `index` (0-4) | HF mode: 0=100ms, 1=500ms, 2=1s, 3=5s, 4=10s |
| `sniffy_counter_hf_config` | `module`, optional: `quantity`, `error_mode`, `averaging`, `gate_time` | quantity: 0=Freq,1=Period; error_mode: 0=Err,1=ErrAVG; averaging: sample count dial; gate_time: 0-4 |
| `sniffy_counter_lf_config` | `module`, optional: `channel`, `quantity`, `multiplier`, `duty_cycle`, `sample_count`, `sample_channel` | channel: 0=CH1,1=CH2; multiplier: 0=1x..3=8x; duty_cycle: 0=off,1=on; `sample_channel` defaults to `channel` if provided, else 0 |
| `sniffy_counter_rat_set_sample_count` | `module`, `value` | Ratio mode reference sample count dial |
| `sniffy_counter_int_config` | `module`, optional: `event_sequence`, `edge_a`, `edge_b`, `timeout` | event_sequence: 0=A→B,1=B→A; edge_a/b: "rising"/"falling"; timeout: seconds |

**Counter state readback** (`sniffy_get_module_state` / `module_start` return):
`mode` (0-3), `mode_name` (HF/LF/Ratio/Intervals).
- **HF**: `hf.quantity` (0=Freq,1=Period), `hf.error_mode`, `hf.gate_time`, `hf.averaging`.
- **LF**: `lf.channel` (0=CH1,1=CH2), `lf.quantity`, `lf.duty_cycle` (0=off,1=on),
  `lf.multiplier`, `lf.sample_count_ch1`, `lf.sample_count_ch2`.
- **Ratio**: `ratio.sample_count`.
- **Intervals**: `intervals.event_sequence`, `intervals.edge_a`/`edge_b`, `intervals.timeout`.

> **Important — `duty_cycle`**: When `lf.duty_cycle` is 1, the LF display shows
> pulse width and duty cycle instead of frequency/period.  If you need a normal
> frequency measurement, you **must** explicitly set `duty_cycle=0` via
> `counter_lf_config`.  The app restores this from the previous session, so
> always check the state after `module_start`.

### Oscilloscope

| Tool | Parameters | Notes |
|------|-----------|-------|
| `sniffy_scope_set_timebase` | `module`, `value` (seconds/div) | Reconfigures sampling |
| `sniffy_scope_set_trigger_mode` | `module`, `mode` | `auto`, `normal`, `single`, `stop` |
| `sniffy_scope_set_trigger_edge` | `module`, `edge` | `rising` or `falling` |
| `sniffy_scope_set_trigger_channel` | `module`, `channel` (0-based) | Which channel triggers |
| `sniffy_scope_set_channels` | `module`, `channel_mask` (bitmask) | Bit 0=CH1, bit 1=CH2, etc. (e.g. 3=CH1+CH2) |
| `sniffy_scope_set_pretrigger` | `module`, `percent` (0-100) | Data before trigger point |
| `sniffy_scope_set_trigger_level` | `module`, `percent` (0-100) | Trigger threshold |
| `sniffy_scope_set_resolution` | `module`, `bits` (8 or 12) | ADC resolution |
| `sniffy_scope_set_sampling_freq` | `module`, `value` (Hz) | Sampling frequency (text input) |
| `sniffy_scope_set_data_length` | `module`, `value` (samples) | Capture data length (text input) |
| `sniffy_scope_add_measurement` | `module`, `type`, `channel` (0-based), optional `channel_b` | Add measurement. Types: `frequency`, `period`, `phase`, `duty`, `low`, `high`, `rms`, `rms_ac`, `mean`, `pkpk`, `max`, `min`. For `phase`, supply both `channel` and `channel_b`. |
| `sniffy_scope_clear_measurements` | `module` | Remove all measurements |
| `sniffy_scope_get_measurements` | `module` | Returns array of live measurement values |

**Scope state readback** (`sniffy_get_module_state`):  
`trigger_mode_name` (auto/normal/stop/single), `real_sampling_rate` (Hz),
`data_length` (samples), `measurements` (array of `{label, value, value_str, channel}`).

### Arbitrary / PWM Generator

Both "Arbitrary generator" (DAC output) and "PWM generator" (modulated
PWM) share the same signal-shaping controls.  Use the exact module name
string.  The PWM generator has additional controls for the PWM carrier
frequency per channel.

#### Common tools (both Arb and PWM)
|------|-----------|-------|
| `sniffy_arbgen_set_frequency` | `module`, `channel` (0-3), `value` (Hz) | Per-channel signal frequency dial |
| `sniffy_arbgen_set_shape` | `module`, `channel` (0-3), `index` | 0=Sine, 1=Saw, 2=Rect, 3=Arb |
| `sniffy_arbgen_set_amplitude` | `module`, `channel` (0-3), `value` (V) | Signal amplitude |
| `sniffy_arbgen_set_offset` | `module`, `channel` (0-3), `value` (V) | DC offset |
| `sniffy_arbgen_set_duty` | `module`, `channel` (0-3), `value` (%) | Duty cycle (Rect/Saw only; hidden for Sine/Arb) |
| `sniffy_arbgen_set_phase` | `module`, `channel` (0-3), `value` (°) | Phase 0-360 degrees |
| `sniffy_arbgen_set_channels` | `module`, `count` (1-4) | Number of active output channels |
| `sniffy_arbgen_set_memory` | `module`, `mode`, opt `length` | 0=Best fit, 1=Long, 2=Custom. For Custom supply `length` (samples). |
| `sniffy_arbgen_set_sweep` | `module`, `enable` (bool), opt `min_freq`, `max_freq`, `sweep_time` | SW frequency sweep on CH1. Freq in Hz, time in seconds. |
| `sniffy_arbgen_set_freq_sync` | `module`, `channel` (1-3), `enabled` (bool) | Sync CH2/3/4 signal frequency to CH1. Channel: 1=CH2, 2=CH3, 3=CH4. |

#### PWM generator only

| Tool | Parameters | Notes |
|------|-----------|-------|
| `sniffy_arbgen_set_pwm_frequency` | `module`, `channel` (0-3), `value` (Hz) | PWM carrier frequency per channel |
| `sniffy_arbgen_set_pwm_freq_sync` | `module`, `channel` (1-3), `enabled` (bool) | Sync CH2-CH4 PWM carrier frequency to CH1 |

> These set GUI widget values. The signal is **not** uploaded to the MCU
> until you call `sniffy_module_run`.  Sequence: `module_start` → set
> params → `module_run`.  To stop generation without closing: `module_halt`.
>
> When CH1 Freq sync is On for a channel, that channel’s frequency dial
> is disabled and locked to CH1's value.  The same applies to PWM Freq
> sync for the PWM carrier frequency.

**State readback** (`sniffy_get_module_state`):
`num_channels`, `memory` (best_fit/long/custom), `custom_length` (if custom),
`sweep_enabled`, `sweep_min_freq`, `sweep_max_freq`, `sweep_time`,
per-channel: `shape`/`shape_name` (sine/saw/rect/arb), `frequency`,
`amplitude`, `offset`, `duty`, `phase`, `freq_sync_ch1` (CH2+ only).
PWM generator adds per-channel: `pwm_frequency`, `pwm_freq_sync_ch1` (CH2+ only).

### Pattern Generator

| Tool | Parameters | Notes |
|------|-----------|-------|
| `sniffy_patgen_set_pattern` | `module`, `index` (0-14) | 0=User, 1=Counter, 2=Binary, 3=Gray, 4=Quadrature, 5=PRBS, 6=PWM, 7=Line code, 8=4B/5B, 9=Johnson, 10=PDM, 11=Parallel bus, 12=UART, 13=SPI, 14=I2C |
| `sniffy_patgen_set_frequency` | `module`, `value` (Hz) | Frequency of the currently selected pattern (patterns 0-11 have a freq dial; 12-14 use baud/clock combos) |
| `sniffy_patgen_set_channels` | `module`, `value` | Channel count / data length. Only for patterns: 0=User (length), 1=Counter (length), 2=Binary (channels), 3=Gray (channels), 8=4B/5B (groups), 9=Johnson (phases), 11=Parallel bus (width) |
| `sniffy_patgen_reset` | `module` | Reset the currently selected pattern to factory defaults |

> **Frequency is per-pattern.** Each of the 15 patterns has its own
> frequency setting.  If you change the pattern with `patgen_set_pattern`,
> the frequency dial switches to that pattern's saved value.  **Always set
> the pattern first, then the frequency.**
>
> **Starting generation:** After configuring, call `sniffy_module_run` to
> upload pattern data and start generation.  Use `sniffy_module_halt` to
> stop without closing the module.

**Pattern Generator state readback** (`sniffy_get_module_state`):
`pattern_index`, `pattern_name`, `frequency` (Hz, if applicable),
`channels` (count/length, if applicable).
Per-pattern extras: Quadrature → `sequence`, PRBS → `prbs_order`,
PWM → `duty`, Line code → `line_code_type`, PDM → `pdm_level`,
UART → `baud`/`data_bits`/`parity`/`stop_bits`/`bit_order`,
SPI → `spi_mode`/`word_size`/`bit_order`/`cs_gating`/`pause_ticks`,
I2C → `clock_freq`/`comm_type`/`addr_mode`/`address`.

### Voltmeter

| Tool | Parameters | Notes |
|------|-----------|-------|
| `sniffy_voltmeter_set_mode` | `module`, `mode` | 0=Normal, 1=Fast |
| `sniffy_voltmeter_set_averaging` | `module`, `value` | Sample count (1-64) |
| `sniffy_voltmeter_set_calc_mode` | `module`, `mode` | 0=Min/Max, 1=Ripple, 2=None |
| `sniffy_voltmeter_set_channels` | `module`, `channel_mask` (bitmask) | Bit 0=CH1, bit 1=CH2, etc. |
| `sniffy_voltmeter_get_readings` | `module` | Returns per-channel live readings |
| `sniffy_voltmeter_set_datalog_file` | `module`, `path` (absolute) | Set log file path (no GUI dialog). File is created/overwritten at start. |
| `sniffy_voltmeter_start_datalog` | `module` | Start logging to the set file |
| `sniffy_voltmeter_stop_datalog` | `module` | Stop logging |

**Voltmeter state readback** (`sniffy_get_module_state`):  
`mode` / `mode_name` (normal/fast), `averaging`, `calc_mode` / `calc_mode_name`
(min_max/ripple/none), `channels` (enable bitmask), `datalog_running`,
`vdd` (supply voltage), `num_channels_enabled`,
`readings` (array of `{voltage, min, max, ripple, frequency, percent}`).

> **Data logging note**: The agent must create or ensure the target .txt/.csv
> file path exists before calling `voltmeter_set_datalog_file`.  The file
> itself will be created/truncated when logging starts.

### Voltage Source

| Tool | Parameters | Notes |
|------|-----------|-------|
| `sniffy_vout_set_voltage` | `module`, `channel` (0-based), `voltage` (V) | Updates dial + display + MCU |

**Voltage Source state readback** (`sniffy_get_module_state`):  
`voltage_ch1`, `voltage_ch2`, … (current dial value per visible channel).

---

## Typical Workflows

### Connect and start measuring

```
1. get_status                     → check if connected
2. scan_devices                   → find attached boards
3. connect_device("F303RE")      → connect by name substring
4. (wait 2-3 s)                   → let device enumerate modules
5. get_status                     → verify connected=true
6. module_start("Counter")       → start Counter — returns state
7. READ the returned state:
   - mode may not be what you need → counter_set_mode to switch
   - lf.duty_cycle may be 1       → counter_lf_config duty_cycle=0
   - lf.channel may be wrong      → counter_lf_config channel=...
   Any setting restored from the previous session must be
   explicitly changed if it differs from what you need.
8. counter_set_mode("Counter", 1) → switch to LF mode
9. counter_lf_config Counter channel=1 duty_cycle=0
                                  → CH2, normal frequency measurement
10. ... do measurements ...
11. module_stop("Counter")
12. disconnect_device
```

### Black-box frequency response test

```
1. get_status                    → confirm device connected
2. module_start("Arbitrary generator")
3. arbgen_set_frequency(module="Arbitrary generator", channel=0, value=1000)
4. arbgen_set_shape(module="Arbitrary generator", channel=0, index=0)
5. module_run("Arbitrary generator")   ← starts signal output
6. module_start("Oscilloscope")
7. For each test frequency:
   a. arbgen_set_frequency("Arbitrary generator", 0, freq)
   b. Wait ~100ms for signal to settle
   c. scope_read_data → measure output amplitude
8. module_halt("Arbitrary generator")
9. module_stop("Oscilloscope")
10. module_stop("Arbitrary generator")
```

### Voltage sweep with measurement

```
1. module_start("Voltage source")
2. module_start("Oscilloscope")
3. For each voltage step:
   a. vout_set_voltage("Voltage source", 0, voltage)
   b. scope_read_data → measure response
4. Stop both modules
```

---

## Tips

- **Prefer GUI-synced tools** (`sniffy_spwm_*`, `sniffy_scope_*`,
  `sniffy_counter_*`, `sniffy_arbgen_*`, `sniffy_patgen_*`,
  `sniffy_voltmeter_*`, `sniffy_vout_*`) over raw `write_command` calls —
  they keep the desktop GUI in perfect sync with your changes.
- Always **check module availability** before starting — firmware may not
  support all modules on every board variant.
- **Resource conflicts** are enforced by the app.  If you get `locked`,
  call `sniffy_list_modules` to find which module is blocking.
- Scope data is captured **passively** — the bridge observes frames on
  the serial bus.  You don't need to poll the MCU directly.
- All parameter changes you make are **visible in the GUI** in real time.
- Integer parameters are encoded as **32-bit little-endian** on the wire;
  the `write_command_int` tool handles this.
- The scope returns voltages in **Volts** and time in **seconds**.
