# sniffy-mcp

Python client & MCP server for the Sniffy Agent Bridge.

## Quick Start

Zero setup. Just pass the AGENT_INSTRUCTIONS.md file as context to the agent (or register the MCP server in your MCP config).

## Install

```bash
pip install -e .             # client only (zero dependencies)
pip install -e ".[mcp]"      # with MCP server (needs 'mcp' package)
```

## Usage — Quick CLI

> **Important**: Run from the `LEO_sniffy/tools/` directory (the parent of
> the `sniffy_mcp` package), NOT from inside `sniffy_mcp/` itself.

Call any `SniffyClient` method from the command line (no MCP package needed):

```bash
cd LEO_sniffy/tools

# ── Core / Status ──────────────────────────────────────────────
python -m sniffy_mcp call ping
python -m sniffy_mcp call get_status
python -m sniffy_mcp call get_system_config

# ── Device Management ─────────────────────────────────────────
python -m sniffy_mcp call scan_devices
python -m sniffy_mcp call list_devices
python -m sniffy_mcp call connect_device 0                  # by index
python -m sniffy_mcp call connect_device "F303RE"           # by name substring
python -m sniffy_mcp call disconnect_device

# ── Module Lifecycle ──────────────────────────────────────────
python -m sniffy_mcp call list_modules
python -m sniffy_mcp call module_start "Sync PWM"
python -m sniffy_mcp call module_stop "Sync PWM"
python -m sniffy_mcp call module_run "Sync PWM"             # start signal generation
python -m sniffy_mcp call module_halt "Sync PWM"            # stop signal, keep module open
python -m sniffy_mcp call get_module_state "Sync PWM"
python -m sniffy_mcp call module_get_config "Sync PWM"

# ── Raw Commands ──────────────────────────────────────────────
python -m sniffy_mcp call write_command "OSCP:TRIG:AUTO"
python -m sniffy_mcp call write_command_int OSCP TRIG 1

# ── Oscilloscope — Configuration ─────────────────────────────
python -m sniffy_mcp call scope_set_timebase Oscilloscope 0.001
python -m sniffy_mcp call scope_set_sampling_freq Oscilloscope 1000000
python -m sniffy_mcp call scope_set_data_length Oscilloscope 4096
python -m sniffy_mcp call scope_set_channels Oscilloscope 3           # bitmask: CH1+CH2
python -m sniffy_mcp call scope_set_resolution Oscilloscope 12        # 8 or 12 bits
python -m sniffy_mcp call scope_set_pretrigger Oscilloscope 50        # percent
python -m sniffy_mcp call scope_set_trigger_mode Oscilloscope auto    # auto|normal|single|stop
python -m sniffy_mcp call scope_set_trigger_edge Oscilloscope rising  # rising|falling
python -m sniffy_mcp call scope_set_trigger_channel Oscilloscope 0
python -m sniffy_mcp call scope_set_trigger_level Oscilloscope 50     # percent

# ── Oscilloscope — Data & Measurements ───────────────────────
python -m sniffy_mcp call scope_read_data
python -m sniffy_mcp call scope_read_data_compact
python -m sniffy_mcp call scope_add_measurement Oscilloscope frequency 0
python -m sniffy_mcp call scope_add_measurement Oscilloscope phase 0 channel_b=1
python -m sniffy_mcp call scope_get_measurements Oscilloscope
python -m sniffy_mcp call scope_clear_measurements Oscilloscope

# ── Oscilloscope — Display Widgets ───────────────────────────
python -m sniffy_mcp call list_displays Oscilloscope
python -m sniffy_mcp call display_save_history Oscilloscope "Frequency"
python -m sniffy_mcp call display_clear_history Oscilloscope "Frequency"
python -m sniffy_mcp call display_set_history_size Oscilloscope "Frequency" 500

# ── Sync PWM ─────────────────────────────────────────────────
python -m sniffy_mcp call spwm_set_frequency "Sync PWM" 0 5000
python -m sniffy_mcp call spwm_set_duty "Sync PWM" 0 50
python -m sniffy_mcp call spwm_set_phase "Sync PWM" 1 90
python -m sniffy_mcp call spwm_set_channel_enable "Sync PWM" 0 1     # enable CH1
python -m sniffy_mcp call spwm_set_step_mode "Sync PWM" 1
python -m sniffy_mcp call spwm_set_equidistant "Sync PWM" 1
python -m sniffy_mcp call spwm_set_invert "Sync PWM" 0 1

# ── Counter ───────────────────────────────────────────────────
python -m sniffy_mcp call counter_set_mode Counter 0                  # 0=HF 1=LF 2=Ratio 3=Intervals
python -m sniffy_mcp call counter_set_gate_time Counter 2             # 0=100ms..4=10s
python -m sniffy_mcp call counter_hf_config Counter quantity=1 gate_time=2
python -m sniffy_mcp call counter_lf_config Counter channel=0 quantity=0 sample_count=100
python -m sniffy_mcp call counter_rat_set_sample_count Counter 1000
python -m sniffy_mcp call counter_int_config Counter event_sequence=0 edge_a=0 timeout=1000

# ── Arb / PWM Generator ──────────────────────────────────────
python -m sniffy_mcp call arbgen_set_frequency "Arb. generator" 0 1000
python -m sniffy_mcp call arbgen_set_shape "Arb. generator" 0 0      # 0=Sine 1=Saw 2=Rect 3=Arb
python -m sniffy_mcp call arbgen_set_amplitude "Arb. generator" 0 1.5
python -m sniffy_mcp call arbgen_set_offset "Arb. generator" 0 0.5
python -m sniffy_mcp call arbgen_set_duty "Arb. generator" 0 50
python -m sniffy_mcp call arbgen_set_phase "Arb. generator" 1 90
python -m sniffy_mcp call arbgen_set_channels "Arb. generator" 2
python -m sniffy_mcp call arbgen_set_memory "Arb. generator" 0       # 0=Best fit 1=Long 2=Custom
python -m sniffy_mcp call arbgen_set_sweep "Arb. generator" 1 100 10000 5.0
python -m sniffy_mcp call arbgen_set_freq_sync "Arb. generator" 1 1  # sync CH2 to CH1
python -m sniffy_mcp call arbgen_set_pwm_frequency "PWM generator" 0 20000
python -m sniffy_mcp call arbgen_set_pwm_freq_sync "PWM generator" 1 1

# ── Pattern Generator ────────────────────────────────────────
python -m sniffy_mcp call patgen_set_pattern "Pattern gen." 0        # pattern index 0–14
python -m sniffy_mcp call patgen_set_frequency "Pattern gen." 1000
python -m sniffy_mcp call patgen_set_channels "Pattern gen." 4
python -m sniffy_mcp call patgen_reset "Pattern gen."

# ── Voltage Source ────────────────────────────────────────────
python -m sniffy_mcp call vout_set_voltage "Voltage source" 0 1.5

# ── Voltmeter ────────────────────────────────────────────────
python -m sniffy_mcp call voltmeter_set_mode Voltmeter 0             # 0=Normal 1=Fast
python -m sniffy_mcp call voltmeter_set_averaging Voltmeter 16
python -m sniffy_mcp call voltmeter_set_calc_mode Voltmeter 0        # 0=Min/Max 1=Ripple 2=None
python -m sniffy_mcp call voltmeter_set_channels Voltmeter 3         # bitmask: CH1+CH2
python -m sniffy_mcp call voltmeter_get_readings Voltmeter
python -m sniffy_mcp call voltmeter_set_datalog_file Voltmeter "C:\data\log.txt"
python -m sniffy_mcp call voltmeter_start_datalog Voltmeter
python -m sniffy_mcp call voltmeter_stop_datalog Voltmeter

# ── Convenience Helpers ───────────────────────────────────────
python -m sniffy_mcp call wait_for_connection                        # blocks until app reachable
python -m sniffy_mcp call wait_for_device                            # blocks until device ready
python -m sniffy_mcp call wait_for_scope_data                        # blocks until fresh snapshot
```

> **Connection timing**: After `connect_device`, wait 2–3 seconds before
> `module_start`.  The MCU needs time to enumerate modules.
> In PowerShell: `Start-Sleep -Seconds 3`.
>
> For automated test systems, use `wait_for_device()` which polls
> `get_status` until `connected == true`:
> ```python
> s.connect_device(name="F303RE")
> s.wait_for_device(timeout=15)   # blocks until ready
> s.module_start("Oscilloscope")
> ```

The CLI uses **SniffyClient method names** (e.g. `module_start`), not MCP
tool names (e.g. `sniffy_module_start`).  Arguments are auto-coerced to
int/float where possible.

## Usage — Direct Python

```python
from sniffy_mcp import SniffyClient

with SniffyClient() as s:
    print(s.get_status())
    s.scan_devices()
    s.connect_device(name="F303RE")
    s.module_start("Oscilloscope")
    s.scope_set_sampling_freq("Oscilloscope", 1000000)
    s.scope_add_measurement("Oscilloscope", "rms", 0)
    data = s.wait_for_scope_data()
    print(s.scope_get_measurements("Oscilloscope"))
    s.module_stop("Oscilloscope")

    s.module_start("Voltmeter")
    s.voltmeter_set_channels("Voltmeter", 3)    # CH1+CH2
    print(s.voltmeter_get_readings("Voltmeter"))
    # Data logging — create the file first if it doesn't exist
    s.voltmeter_set_datalog_file("Voltmeter", "C:\\data\\log.txt")
    s.voltmeter_start_datalog("Voltmeter")
    import time; time.sleep(5)                   # collect 5 seconds of data
    s.voltmeter_stop_datalog("Voltmeter")
    s.module_stop("Voltmeter")
```

## Usage — MCP Server

```bash
pip install -e ".[mcp]"     # needs 'mcp' package
python -m sniffy_mcp         # starts MCP stdio server
```

See [AGENT_INSTRUCTIONS.md](AGENT_INSTRUCTIONS.md) for the full protocol
reference and workflow examples.
