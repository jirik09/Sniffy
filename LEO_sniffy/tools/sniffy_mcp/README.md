# sniffy-mcp

Python client & MCP server for the Sniffy Agent Bridge.

## Quick Start

Zero setup. Just pass the AGENT_INSTRUCTIONS.md file as context to the agent (or register the MCP server in your MCP config).

## Install

```bash
pip install -e .             # client only (zero dependencies)
pip install -e ".[mcp]"      # with MCP server (needs 'mcp' package)
```

## Usage — Direct Python

```python
from sniffy_mcp import SniffyClient

with SniffyClient() as s:
    print(s.get_status())
    s.module_start("Oscilloscope")
    data = s.wait_for_scope_data()
    s.module_stop("Oscilloscope")
```

## Usage — Quick CLI

Call any `SniffyClient` method from the command line (no MCP package needed):

```bash
python -m sniffy_mcp call get_status
python -m sniffy_mcp call module_start "Sync PWM"
python -m sniffy_mcp call module_stop "Sync PWM"
python -m sniffy_mcp call display_set_history_size Counter HighFreqCounter 555
python -m sniffy_mcp call spwm_set_frequency "Sync PWM" 0 5000
```

Arguments are auto-coerced to int/float where possible.

## Usage — MCP Server

```bash
pip install -e ".[mcp]"     # needs 'mcp' package
python -m sniffy_mcp         # starts MCP stdio server
```

See [AGENT_INSTRUCTIONS.md](AGENT_INSTRUCTIONS.md) for the full protocol
reference and workflow examples.
