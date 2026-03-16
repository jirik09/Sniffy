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
python -m sniffy_mcp call get_status
python -m sniffy_mcp call scan_devices
python -m sniffy_mcp call connect_device 0                  # by index
python -m sniffy_mcp call connect_device "F303RE"           # by name substring
python -m sniffy_mcp call module_start "Sync PWM"
python -m sniffy_mcp call module_stop "Sync PWM"
python -m sniffy_mcp call spwm_set_frequency "Sync PWM" 0 5000
python -m sniffy_mcp call spwm_set_step_mode "Sync PWM" 1
python -m sniffy_mcp call spwm_set_invert "Sync PWM" 0 1
python -m sniffy_mcp call counter_hf_config Counter quantity=1 gate_time=2
python -m sniffy_mcp call scope_set_sampling_freq Oscilloscope 1000000
python -m sniffy_mcp call scope_set_data_length Oscilloscope 4096
python -m sniffy_mcp call scope_add_measurement Oscilloscope frequency 0
python -m sniffy_mcp call scope_add_measurement Oscilloscope phase 0 channel_b=1
python -m sniffy_mcp call scope_get_measurements Oscilloscope
python -m sniffy_mcp call scope_clear_measurements Oscilloscope
python -m sniffy_mcp call voltmeter_set_channels Voltmeter 3          # CH1+CH2
python -m sniffy_mcp call voltmeter_get_readings Voltmeter
python -m sniffy_mcp call voltmeter_set_datalog_file Voltmeter "C:\data\log.txt"
python -m sniffy_mcp call voltmeter_start_datalog Voltmeter
python -m sniffy_mcp call voltmeter_stop_datalog Voltmeter
python -m sniffy_mcp call vout_set_voltage "Voltage source" 0 1.5
```

> **Connection timing**: After `connect_device`, wait 2–3 seconds before
> `module_start`.  The MCU needs time to enumerate modules.
> In PowerShell: `Start-Sleep -Seconds 3`.

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
