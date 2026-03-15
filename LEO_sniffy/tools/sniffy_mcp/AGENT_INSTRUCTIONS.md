# Sniffy Agent Instructions

You are controlling a **Sniffy** hardware test instrument — a PC application
connected to an STM32 Nucleo MCU via USB-serial.  The MCU provides:
oscilloscope, arbitrary generator, PWM generator, pattern generator,
counter, voltage source, and voltmeter modules.

You interact with the hardware through the **AgentBridge** — an IPC server
running inside the Sniffy desktop app.  Every action you take is mirrored
in the GUI so the user can observe in real time.

---

## Connection

The bridge listens on the named pipe **`sniffy-agent`**.
Use the MCP tools (prefixed `sniffy_`) or the Python client directly.

### Quick CLI (no MCP package needed)

```bash
cd LEO_sniffy/tools/sniffy_mcp
python -m sniffy_mcp call get_status
python -m sniffy_mcp call module_start "Sync PWM"
python -m sniffy_mcp call display_set_history_size Counter HighFreqCounter 555
```

### Python script

```python
from sniffy_mcp import SniffyClient
with SniffyClient() as s:
    print(s.get_status())
```

**Always start with** `sniffy_get_status` (or `get_status` in Python / CLI)
- The device is connected (`connected: true`)
- You are not in demo mode (or note the limitations if you are)
- Which modules are available and their current state

---

## Module Lifecycle

Each module follows: **stopped → running → paused → stopped**.

| Action | Tool | Notes |
|--------|------|-------|
| Open & start | `sniffy_module_start` | Returns `state` object with session-restored config |
| Stop & close | `sniffy_module_stop` | Releases hardware resources |
| Check state | `sniffy_list_modules` | `status` field per module |
| Read GUI state | `sniffy_get_module_state` | Current mode, dials, buttons — use any time |

Modules that share hardware resources **cannot run simultaneously** — the
app enforces this automatically.  If a module shows `locked`, stop the
conflicting module first.

> **Session persistence**: The app restores the last-used configuration
> (mode, dials, tabs) from a session file on launch.  **Always check the
> `state` returned by `module_start`** (or call `get_module_state`) to
> learn which mode/tab is active — do not assume defaults.  Then use the
> appropriate setter to change any setting that differs from what you need.

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

### Sync PWM

| Tool | Parameters | Notes |
|------|-----------|-------|
| `sniffy_spwm_set_frequency` | `module`, `channel` (0-3), `value` (Hz) | Updates dial + MCU |
| `sniffy_spwm_set_duty` | `module`, `channel` (0-3), `value` (%) | Duty cycle 0-100 |
| `sniffy_spwm_set_phase` | `module`, `channel` (0-3), `value` (°) | Phase 0-360 degrees |
| `sniffy_spwm_set_channel_enable` | `module`, `channel` (0-3), `enabled` (bool) | ON/OFF toggle |

**Workflow example — Set CH1 to 5 kHz, 50% duty, then start:**
```
1. sniffy_module_start("Sync PWM")
2. sniffy_spwm_set_frequency(module="Sync PWM", channel=0, value=5000)
3. sniffy_spwm_set_duty(module="Sync PWM", channel=0, value=50)
4. sniffy_spwm_set_channel_enable(module="Sync PWM", channel=0, enabled=true)
```

### Counter

| Tool | Parameters | Notes |
|------|-----------|-------|
| `sniffy_counter_set_mode` | `module`, `mode` (0-3) | 0=HF, 1=LF, 2=Ratio, 3=Intervals |
| `sniffy_counter_set_gate_time` | `module`, `index` (0-4) | HF mode: 0=100ms, 1=500ms, 2=1s, 3=5s, 4=10s |
| `sniffy_counter_hf_config` | `module`, optional: `quantity`, `error_mode`, `averaging`, `gate_time` | quantity: 0=Freq,1=Period; error_mode: 0=Err,1=ErrAVG; averaging: sample count dial; gate_time: 0-4 |
| `sniffy_counter_lf_config` | `module`, optional: `channel`, `quantity`, `multiplier`, `duty_cycle`, `sample_count`, `sample_channel` | channel: 0=CH1,1=CH2; multiplier: 0=1x..3=8x; duty_cycle: 0=off,1=on; sample_channel selects which dial |
| `sniffy_counter_rat_set_sample_count` | `module`, `value` | Ratio mode reference sample count dial |
| `sniffy_counter_int_config` | `module`, optional: `event_sequence`, `edge_a`, `edge_b`, `timeout` | event_sequence: 0=A→B,1=B→A; edge_a/b: "rising"/"falling"; timeout: seconds |

### Oscilloscope

| Tool | Parameters | Notes |
|------|-----------|-------|
| `sniffy_scope_set_timebase` | `module`, `value` (seconds/div) | Reconfigures sampling |
| `sniffy_scope_set_trigger_mode` | `module`, `mode` | `auto`, `auto_fast`, `normal`, `single`, `stop` |
| `sniffy_scope_set_trigger_edge` | `module`, `edge` | `rising` or `falling` |
| `sniffy_scope_set_trigger_channel` | `module`, `channel` (0-based) | Which channel triggers |
| `sniffy_scope_set_channels` | `module`, `channel_mask` (bitmask) | Bit 0=CH1, bit 1=CH2, etc. (e.g. 3=CH1+CH2) |
| `sniffy_scope_set_pretrigger` | `module`, `percent` (0-100) | Data before trigger point |
| `sniffy_scope_set_trigger_level` | `module`, `percent` (0-100) | Trigger threshold |
| `sniffy_scope_set_resolution` | `module`, `bits` (8 or 12) | ADC resolution |

### Arbitrary / PWM Generator

Both "Arbitrary generator" (DAC output) and "PWM generator" (modulated
PWM) share the same controls.  Use the exact module name string.

| Tool | Parameters | Notes |
|------|-----------|-------|
| `sniffy_arbgen_set_frequency` | `module`, `channel` (0-3), `value` (Hz) | Per-channel frequency dial |
| `sniffy_arbgen_set_shape` | `module`, `channel` (0-3), `index` | 0=Sine, 1=Saw, 2=Rect, 3=Arb |
| `sniffy_arbgen_set_amplitude` | `module`, `channel` (0-3), `value` (V) | Signal amplitude |
| `sniffy_arbgen_set_offset` | `module`, `channel` (0-3), `value` (V) | DC offset |
| `sniffy_arbgen_set_duty` | `module`, `channel` (0-3), `value` (%) | Duty cycle (Rect/Saw) |
| `sniffy_arbgen_set_phase` | `module`, `channel` (0-3), `value` (°) | Phase 0-360 degrees |
| `sniffy_arbgen_set_channels` | `module`, `count` (1-4) | Number of active output channels |

> These set GUI widget values. The signal is uploaded to the MCU when the
> module is started with `sniffy_module_start`.

### Pattern Generator

| Tool | Parameters | Notes |
|------|-----------|-------|
| `sniffy_patgen_set_pattern` | `module`, `index` (0-14) | 0=User, 1=Counter, 2=Binary, 3=Gray, 4=Quadrature, 5=PRBS, 6=PWM, 7=Line code, 8=4B/5B, 9=Johnson, 10=PDM, 11=Parallel bus, 12=UART, 13=SPI, 14=I2C |
| `sniffy_patgen_set_frequency` | `module`, `value` (Hz) | Frequency of the currently selected pattern |

### Voltmeter

| Tool | Parameters | Notes |
|------|-----------|-------|
| `sniffy_voltmeter_set_mode` | `module`, `mode` | 0=Normal, 1=Fast |
| `sniffy_voltmeter_set_averaging` | `module`, `value` | Sample count (1-64) |
| `sniffy_voltmeter_set_calc_mode` | `module`, `mode` | 0=Min/Max, 1=Ripple, 2=None |

### Voltage Source

| Tool | Parameters | Notes |
|------|-----------|-------|
| `sniffy_vout_set_voltage` | `module`, `channel` (0-based), `voltage` (V) | Updates display + MCU |

---

## Typical Workflows

### Black-box frequency response test

```
1. sniffy_get_status             → confirm device connected
2. sniffy_module_start("Arbitrary generator")
3. sniffy_module_start("Oscilloscope")
4. For each test frequency:
   a. write_command_int("GENS", "FREQ", (freq << 8) | 1)
   b. Wait ~100ms for signal to settle
   c. scope_read_data → measure output amplitude
5. sniffy_module_stop("Oscilloscope")
6. sniffy_module_stop("Arbitrary generator")
```

### Voltage sweep with measurement

```
1. sniffy_module_start("Voltage source")
2. sniffy_module_start("Oscilloscope")
3. For each voltage step:
   a. Set voltage via VOUT commands
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
