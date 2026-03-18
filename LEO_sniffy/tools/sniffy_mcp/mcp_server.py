"""MCP (Model Context Protocol) server exposing Sniffy tools.

Run with::

    python -m sniffy_mcp.mcp_server

or via the ``sniffy-mcp`` entry-point after ``pip install -e .``

Requires the ``mcp`` package (``pip install mcp``).
If ``mcp`` is not installed the module raises ImportError on import.
"""

from __future__ import annotations

import json
import sys

try:
    from mcp.server import Server
    from mcp.server.stdio import run_server
    from mcp.types import Tool, TextContent
except ImportError:
    print(
        "The 'mcp' package is required for the MCP server.\n"
        "Install it with:  pip install mcp",
        file=sys.stderr,
    )
    raise

from .client import SniffyClient

# ── Global Sniffy client (lazy connect) ──────────────────

_client: SniffyClient | None = None


def _get_client() -> SniffyClient:
    global _client
    if _client is None:
        _client = SniffyClient()
        _client.connect()
    return _client


# ── MCP server definition ────────────────────────────────

app = Server("sniffy-mcp")


@app.list_tools()
async def list_tools() -> list[Tool]:
    return [
        Tool(
            name="sniffy_ping",
            description="Health-check: verify the Sniffy app is reachable.",
            inputSchema={"type": "object", "properties": {}},
        ),
        Tool(
            name="sniffy_get_status",
            description=(
                "Get device connection status, demo mode flag, device name, "
                "and a list of all modules with their current state."
            ),
            inputSchema={"type": "object", "properties": {}},
        ),
        Tool(
            name="sniffy_get_system_config",
            description=(
                "Get full hardware configuration: device info (MCU, clock, FW version), "
                "per-module capabilities (channels, sampling rates, pins), "
                "and resource conflict matrix showing which modules cannot run simultaneously."
            ),
            inputSchema={"type": "object", "properties": {}},
        ),
        # ── Device management ──
        Tool(
            name="sniffy_scan_devices",
            description=(
                "Probe all COM ports for Sniffy-compatible boards. "
                "Returns a list of found devices with index, name, and port. "
                "Must be called before connect_device if no devices are in the list. "
                "Fails if already connected — disconnect first."
            ),
            inputSchema={"type": "object", "properties": {}},
        ),
        Tool(
            name="sniffy_list_devices",
            description=(
                "Return the cached device list from the last scan (no re-scan). "
                "Also reports whether a device is currently connected."
            ),
            inputSchema={"type": "object", "properties": {}},
        ),
        Tool(
            name="sniffy_connect_device",
            description=(
                "Connect to a device by its index in the scanned list OR by a name "
                "substring (e.g. 'F303RE'). Call scan_devices first to populate the list."
            ),
            inputSchema={
                "type": "object",
                "properties": {
                    "index": {
                        "type": "integer",
                        "description": "Device index from scan_devices result.",
                    },
                    "name": {
                        "type": "string",
                        "description": "Substring match against device name (e.g. 'F303RE').",
                    },
                },
            },
        ),
        Tool(
            name="sniffy_disconnect_device",
            description="Disconnect the currently connected device.",
            inputSchema={"type": "object", "properties": {}},
        ),
        # ── Module lifecycle ──
        Tool(
            name="sniffy_list_modules",
            description="List all available instrument modules and their status.",
            inputSchema={"type": "object", "properties": {}},
        ),
        Tool(
            name="sniffy_module_start",
            description=(
                "Open and initialise a module — HW is configured but signal generation "
                "does NOT start.  For Sync PWM, Arbitrary/PWM Generator and Pattern "
                "Generator, call sniffy_module_run afterwards to begin output. "
                "Returns an error if a conflicting module is already active (resource overlap)."
            ),
            inputSchema={
                "type": "object",
                "properties": {
                    "module": {
                        "type": "string",
                        "description": "Module name (e.g. 'Oscilloscope', 'Arbitrary generator', 'Counter').",
                    }
                },
                "required": ["module"],
            },
        ),
        Tool(
            name="sniffy_module_stop",
            description="Stop (close) a running module and release hardware resources.",
            inputSchema={
                "type": "object",
                "properties": {
                    "module": {"type": "string", "description": "Module name."}
                },
                "required": ["module"],
            },
        ),
        Tool(
            name="sniffy_module_run",
            description=(
                "Start signal generation — clicks the internal Start/Generate button. "
                "Only needed for Sync PWM, Arbitrary/PWM Generator and Pattern Generator. "
                "Counter and Scope start automatically on module_start. "
                "Returns generation_status: 'running' (SPWM, instant) or 'uploading' (ArbGen/PatGen, "
                "data being sent to MCU). Poll get_module_state until generation_status becomes 'running' "
                "before starting any timed measurement."
            ),
            inputSchema={
                "type": "object",
                "properties": {
                    "module": {"type": "string", "description": "Module name."}
                },
                "required": ["module"],
            },
        ),
        Tool(
            name="sniffy_module_halt",
            description=(
                "Stop signal generation without closing the module. "
                "The module window stays open and parameters can be changed before calling module_run again."
            ),
            inputSchema={
                "type": "object",
                "properties": {
                    "module": {"type": "string", "description": "Module name."}
                },
                "required": ["module"],
            },
        ),
        Tool(
            name="sniffy_get_module_state",
            description=(
                "Read the current GUI state of a running module. "
                "Returns mode/tab, dial values, button selections etc. "
                "Use after module_start to check session-restored state, "
                "or at any time to verify current settings."
            ),
            inputSchema={
                "type": "object",
                "properties": {
                    "module": {"type": "string", "description": "Module name."}
                },
                "required": ["module"],
            },
        ),
        Tool(
            name="sniffy_write_command",
            description=(
                "Send a raw ASCII protocol command to the MCU. "
                "Use the 4-char module prefix, colon-separated fields. "
                "Example: 'OSCP:TRIG:AUTO' or 'GENS:MODE:DAC_'."
            ),
            inputSchema={
                "type": "object",
                "properties": {
                    "command": {
                        "type": "string",
                        "description": "Protocol command string (semicolon auto-appended).",
                    }
                },
                "required": ["command"],
            },
        ),
        Tool(
            name="sniffy_write_command_int",
            description=(
                "Send MODULE:FEATURE with a 32-bit integer parameter. "
                "Used for numeric settings like sampling frequency or trigger level."
            ),
            inputSchema={
                "type": "object",
                "properties": {
                    "prefix": {
                        "type": "string",
                        "description": "4-char module prefix (e.g. 'OSCP').",
                    },
                    "feature": {
                        "type": "string",
                        "description": "4-char feature code (e.g. 'FREQ', 'LEVL').",
                    },
                    "value": {
                        "type": "integer",
                        "description": "32-bit integer parameter value.",
                    },
                },
                "required": ["prefix", "feature", "value"],
            },
        ),
        Tool(
            name="sniffy_scope_read_data",
            description=(
                "Read the last captured oscilloscope data. Returns per-channel "
                "x (time in seconds) and y (voltage in volts) arrays, plus "
                "sampling rate and resolution."
            ),
            inputSchema={"type": "object", "properties": {}},
        ),
        Tool(
            name="sniffy_module_get_config",
            description="Get a module's current configuration as base64 blob.",
            inputSchema={
                "type": "object",
                "properties": {
                    "module": {"type": "string", "description": "Module name."}
                },
                "required": ["module"],
            },
        ),
        Tool(
            name="sniffy_list_displays",
            description=(
                "List WidgetDisplay names inside a module. "
                "Use the returned names with save/clear/resize history tools. "
                "Counter displays: HighFreqCounter, LowFreqCh1Counter, LowFreqCh2Counter, "
                "RatioCounter, IntervalsCounter."
            ),
            inputSchema={
                "type": "object",
                "properties": {
                    "module": {"type": "string", "description": "Module name (e.g. 'Counter')."}
                },
                "required": ["module"],
            },
        ),
        Tool(
            name="sniffy_display_save_history",
            description="Save a display's measurement history to a text file (log_<name>.txt).",
            inputSchema={
                "type": "object",
                "properties": {
                    "module": {"type": "string", "description": "Module name."},
                    "display": {"type": "string", "description": "Display name (from list_displays)."},
                },
                "required": ["module", "display"],
            },
        ),
        Tool(
            name="sniffy_display_clear_history",
            description="Clear a display's measurement history (chart and list).",
            inputSchema={
                "type": "object",
                "properties": {
                    "module": {"type": "string", "description": "Module name."},
                    "display": {"type": "string", "description": "Display name."},
                },
                "required": ["module", "display"],
            },
        ),
        Tool(
            name="sniffy_display_set_history_size",
            description="Set the number of history samples kept (100-1000, same as the dial in the GUI).",
            inputSchema={
                "type": "object",
                "properties": {
                    "module": {"type": "string", "description": "Module name."},
                    "display": {"type": "string", "description": "Display name."},
                    "size": {"type": "integer", "description": "Sample count (100-1000)."},
                },
                "required": ["module", "display", "size"],
            },
        ),
        # ── GUI-synced high-level tools ───────────────────────
        Tool(
            name="sniffy_spwm_set_frequency",
            description="Set Sync PWM channel frequency in Hz. Updates GUI dial + sends MCU command.",
            inputSchema={
                "type": "object",
                "properties": {
                    "module": {"type": "string", "description": "Module name (e.g. 'Sync PWM')."},
                    "channel": {"type": "integer", "description": "Channel index 0-3."},
                    "value": {"type": "number", "description": "Frequency in Hz."},
                },
                "required": ["module", "channel", "value"],
            },
        ),
        Tool(
            name="sniffy_spwm_set_duty",
            description="Set Sync PWM channel duty cycle in %. Updates GUI dial + sends MCU command.",
            inputSchema={
                "type": "object",
                "properties": {
                    "module": {"type": "string", "description": "Module name (e.g. 'Sync PWM')."},
                    "channel": {"type": "integer", "description": "Channel index 0-3."},
                    "value": {"type": "number", "description": "Duty cycle (0-100 %)."},
                },
                "required": ["module", "channel", "value"],
            },
        ),
        Tool(
            name="sniffy_spwm_set_phase",
            description="Set Sync PWM channel phase in degrees 0-360. Updates GUI dial + sends MCU command.",
            inputSchema={
                "type": "object",
                "properties": {
                    "module": {"type": "string", "description": "Module name (e.g. 'Sync PWM')."},
                    "channel": {"type": "integer", "description": "Channel index 0-3."},
                    "value": {"type": "number", "description": "Phase in degrees (0-360)."},
                },
                "required": ["module", "channel", "value"],
            },
        ),
        Tool(
            name="sniffy_spwm_set_channel_enable",
            description="Enable or disable a Sync PWM channel. Updates GUI switch + sends MCU command.",
            inputSchema={
                "type": "object",
                "properties": {
                    "module": {"type": "string", "description": "Module name (e.g. 'Sync PWM')."},
                    "channel": {"type": "integer", "description": "Channel index 0-3."},
                    "enabled": {"type": "boolean", "description": "true=ON, false=OFF."},
                },
                "required": ["module", "channel", "enabled"],
            },
        ),
        Tool(
            name="sniffy_spwm_set_step_mode",
            description="Switch Sync PWM between Continuous (false) and Step (true) mode.",
            inputSchema={
                "type": "object",
                "properties": {
                    "module": {"type": "string", "description": "Module name (e.g. 'Sync PWM')."},
                    "step": {"type": "boolean", "description": "false=Continuous, true=Step."},
                },
                "required": ["module", "step"],
            },
        ),
        Tool(
            name="sniffy_spwm_set_equidistant",
            description="Enable or disable equidistant phase distribution across Sync PWM channels.",
            inputSchema={
                "type": "object",
                "properties": {
                    "module": {"type": "string", "description": "Module name (e.g. 'Sync PWM')."},
                    "enabled": {"type": "boolean", "description": "true=enable, false=disable."},
                },
                "required": ["module", "enabled"],
            },
        ),
        Tool(
            name="sniffy_spwm_set_invert",
            description="Enable or disable waveform inversion for a specific Sync PWM channel.",
            inputSchema={
                "type": "object",
                "properties": {
                    "module": {"type": "string", "description": "Module name (e.g. 'Sync PWM')."},
                    "channel": {"type": "integer", "description": "Channel index 0-3."},
                    "enabled": {"type": "boolean", "description": "true=inverted, false=normal."},
                },
                "required": ["module", "channel", "enabled"],
            },
        ),
        Tool(
            name="sniffy_counter_set_mode",
            description=(
                "Switch Counter measurement mode. Updates GUI tab + sends MCU command. "
                "Modes: 0=High Frequency, 1=Low Frequency, 2=Ratio, 3=Intervals."
            ),
            inputSchema={
                "type": "object",
                "properties": {
                    "module": {"type": "string", "description": "Module name (e.g. 'Counter')."},
                    "mode": {"type": "integer", "description": "Mode index (0-3)."},
                },
                "required": ["module", "mode"],
            },
        ),
        Tool(
            name="sniffy_counter_set_gate_time",
            description=(
                "Set HF Counter gate time. Updates GUI buttons + sends MCU command. "
                "Index: 0=100ms, 1=500ms, 2=1s, 3=5s, 4=10s."
            ),
            inputSchema={
                "type": "object",
                "properties": {
                    "module": {"type": "string", "description": "Module name (e.g. 'Counter')."},
                    "index": {"type": "integer", "description": "Gate time index (0-4)."},
                },
                "required": ["module", "index"],
            },
        ),
        Tool(
            name="sniffy_scope_set_timebase",
            description="Set oscilloscope time/div in seconds. Reconfigures MCU sampling.",
            inputSchema={
                "type": "object",
                "properties": {
                    "module": {"type": "string", "description": "Module name (e.g. 'Oscilloscope')."},
                    "value": {"type": "number", "description": "Time per division in seconds."},
                },
                "required": ["module", "value"],
            },
        ),
        Tool(
            name="sniffy_scope_set_trigger_mode",
            description="Set oscilloscope trigger mode: auto, normal, single, or stop.",
            inputSchema={
                "type": "object",
                "properties": {
                    "module": {"type": "string", "description": "Module name (e.g. 'Oscilloscope')."},
                    "mode": {"type": "string", "description": "Trigger mode (auto|normal|single|stop)."},
                },
                "required": ["module", "mode"],
            },
        ),
        Tool(
            name="sniffy_scope_set_trigger_edge",
            description="Set oscilloscope trigger edge: rising or falling.",
            inputSchema={
                "type": "object",
                "properties": {
                    "module": {"type": "string", "description": "Module name (e.g. 'Oscilloscope')."},
                    "edge": {"type": "string", "description": "Trigger edge (rising|falling)."},
                },
                "required": ["module", "edge"],
            },
        ),
        Tool(
            name="sniffy_scope_set_trigger_channel",
            description="Set which channel the oscilloscope triggers on (0-based index).",
            inputSchema={
                "type": "object",
                "properties": {
                    "module": {"type": "string", "description": "Module name (e.g. 'Oscilloscope')."},
                    "channel": {"type": "integer", "description": "Channel index (0-based)."},
                },
                "required": ["module", "channel"],
            },
        ),
        Tool(
            name="sniffy_scope_set_channels",
            description="Enable oscilloscope channels by bitmask (bit 0=CH1, bit 1=CH2, etc.).",
            inputSchema={
                "type": "object",
                "properties": {
                    "module": {"type": "string", "description": "Module name (e.g. 'Oscilloscope')."},
                    "channel_mask": {"type": "integer", "description": "Bitmask of enabled channels (e.g. 3 = CH1+CH2)."},
                },
                "required": ["module", "channel_mask"],
            },
        ),
        Tool(
            name="sniffy_scope_set_pretrigger",
            description="Set oscilloscope pretrigger percentage (0-100).",
            inputSchema={
                "type": "object",
                "properties": {
                    "module": {"type": "string", "description": "Module name (e.g. 'Oscilloscope')."},
                    "percent": {"type": "number", "description": "Pretrigger percentage (0-100)."},
                },
                "required": ["module", "percent"],
            },
        ),
        Tool(
            name="sniffy_scope_set_trigger_level",
            description="Set oscilloscope trigger level percentage (0-100).",
            inputSchema={
                "type": "object",
                "properties": {
                    "module": {"type": "string", "description": "Module name (e.g. 'Oscilloscope')."},
                    "percent": {"type": "number", "description": "Trigger level percentage (0-100)."},
                },
                "required": ["module", "percent"],
            },
        ),
        Tool(
            name="sniffy_scope_set_resolution",
            description="Set oscilloscope ADC resolution (8 or 12 bits).",
            inputSchema={
                "type": "object",
                "properties": {
                    "module": {"type": "string", "description": "Module name (e.g. 'Oscilloscope')."},
                    "bits": {"type": "integer", "description": "ADC resolution (8 or 12)."},
                },
                "required": ["module", "bits"],
            },
        ),
        Tool(
            name="sniffy_scope_set_sampling_freq",
            description="Set oscilloscope custom sampling frequency in Hz.",
            inputSchema={
                "type": "object",
                "properties": {
                    "module": {"type": "string", "description": "Module name (e.g. 'Oscilloscope')."},
                    "value": {"type": "integer", "description": "Sampling frequency in Hz."},
                },
                "required": ["module", "value"],
            },
        ),
        Tool(
            name="sniffy_scope_set_data_length",
            description="Set oscilloscope custom data length in samples.",
            inputSchema={
                "type": "object",
                "properties": {
                    "module": {"type": "string", "description": "Module name (e.g. 'Oscilloscope')."},
                    "value": {"type": "integer", "description": "Data length in samples."},
                },
                "required": ["module", "value"],
            },
        ),
        Tool(
            name="sniffy_scope_add_measurement",
            description=(
                "Add an oscilloscope measurement. Types: frequency, period, phase, duty, "
                "low, high, rms, rms_ac, mean, pkpk, max, min. "
                "For phase, specify channel (1st) and channel_b (2nd)."
            ),
            inputSchema={
                "type": "object",
                "properties": {
                    "module": {"type": "string", "description": "Module name (e.g. 'Oscilloscope')."},
                    "type": {"type": "string", "description": "Measurement type."},
                    "channel": {"type": "integer", "description": "Channel index (0-based, default 0)."},
                    "channel_b": {"type": "integer", "description": "Second channel for phase (0-based, default 1)."},
                },
                "required": ["module", "type"],
            },
        ),
        Tool(
            name="sniffy_scope_clear_measurements",
            description="Clear all oscilloscope measurements.",
            inputSchema={
                "type": "object",
                "properties": {
                    "module": {"type": "string", "description": "Module name (e.g. 'Oscilloscope')."},
                },
                "required": ["module"],
            },
        ),
        Tool(
            name="sniffy_scope_get_measurements",
            description="Read current oscilloscope measurement results (label, value, channel for each).",
            inputSchema={
                "type": "object",
                "properties": {
                    "module": {"type": "string", "description": "Module name (e.g. 'Oscilloscope')."},
                },
                "required": ["module"],
            },
        ),
        Tool(
            name="sniffy_vout_set_voltage",
            description="Set DAC voltage output for a channel. Updates GUI display + sends MCU command.",
            inputSchema={
                "type": "object",
                "properties": {
                    "module": {"type": "string", "description": "Module name (e.g. 'Voltage source')."},
                    "channel": {"type": "integer", "description": "Channel index (0-based)."},
                    "voltage": {"type": "number", "description": "Output voltage in volts."},
                },
                "required": ["module", "channel", "voltage"],
            },
        ),
        # Counter extended configs
        Tool(
            name="sniffy_counter_hf_config",
            description=(
                "Configure HF Counter tab. All params optional. "
                "quantity: 0=Frequency,1=Period. error_mode: 0=Err,1=Err AVG. "
                "averaging: sample count. gate_time: 0=100ms,1=500ms,2=1s,3=5s,4=10s."
            ),
            inputSchema={
                "type": "object",
                "properties": {
                    "module": {"type": "string"},
                    "quantity": {"type": "integer", "description": "0=Frequency, 1=Period."},
                    "error_mode": {"type": "integer", "description": "0=Err, 1=Err AVG."},
                    "averaging": {"type": "number", "description": "Averaging sample count."},
                    "gate_time": {"type": "integer", "description": "Gate time index (0-4)."},
                },
                "required": ["module"],
            },
        ),
        Tool(
            name="sniffy_counter_lf_config",
            description=(
                "Configure LF Counter tab. All params optional. "
                "channel: 0=CH1,1=CH2. quantity: 0=Freq,1=Period. "
                "multiplier: 0=1x,1=2x,2=4x,3=8x. duty_cycle: 0=Disable,1=Enable. "
                "sample_count + sample_channel: dial value & which channel (0/1)."
            ),
            inputSchema={
                "type": "object",
                "properties": {
                    "module": {"type": "string"},
                    "channel": {"type": "integer"},
                    "quantity": {"type": "integer"},
                    "multiplier": {"type": "integer"},
                    "duty_cycle": {"type": "integer"},
                    "sample_count": {"type": "number"},
                    "sample_channel": {"type": "integer", "description": "0=CH1, 1=CH2."},
                },
                "required": ["module"],
            },
        ),
        Tool(
            name="sniffy_counter_rat_set_sample_count",
            description="Set Ratio counter reference sample count dial.",
            inputSchema={
                "type": "object",
                "properties": {
                    "module": {"type": "string"},
                    "value": {"type": "number", "description": "Sample count."},
                },
                "required": ["module", "value"],
            },
        ),
        Tool(
            name="sniffy_counter_int_config",
            description=(
                "Configure Intervals Counter. All params optional. "
                "event_sequence: 0=A\u2192B,1=B\u2192A. edge_a/edge_b: 'rising'/'falling'. timeout: seconds."
            ),
            inputSchema={
                "type": "object",
                "properties": {
                    "module": {"type": "string"},
                    "event_sequence": {"type": "integer", "description": "0=A->B, 1=B->A."},
                    "edge_a": {"type": "string", "description": "'rising' or 'falling'."},
                    "edge_b": {"type": "string", "description": "'rising' or 'falling'."},
                    "timeout": {"type": "number", "description": "Timeout in seconds."},
                },
                "required": ["module"],
            },
        ),
        # Arbitrary / PWM Generator
        Tool(
            name="sniffy_arbgen_set_frequency",
            description="Set Arb/PWM generator signal frequency (Hz) per channel. Updates GUI dial.",
            inputSchema={
                "type": "object",
                "properties": {
                    "module": {"type": "string", "description": "'Arbitrary generator' or 'PWM generator'."},
                    "channel": {"type": "integer", "description": "Channel index 0-3."},
                    "value": {"type": "number", "description": "Frequency in Hz."},
                },
                "required": ["module", "channel", "value"],
            },
        ),
        Tool(
            name="sniffy_arbgen_set_shape",
            description="Set signal shape per channel: 0=Sine, 1=Saw, 2=Rect, 3=Arb.",
            inputSchema={
                "type": "object",
                "properties": {
                    "module": {"type": "string"},
                    "channel": {"type": "integer"},
                    "index": {"type": "integer", "description": "0=Sine, 1=Saw, 2=Rect, 3=Arb."},
                },
                "required": ["module", "channel", "index"],
            },
        ),
        Tool(
            name="sniffy_arbgen_set_amplitude",
            description="Set signal amplitude (V) per channel.",
            inputSchema={
                "type": "object",
                "properties": {
                    "module": {"type": "string"},
                    "channel": {"type": "integer"},
                    "value": {"type": "number", "description": "Amplitude in volts."},
                },
                "required": ["module", "channel", "value"],
            },
        ),
        Tool(
            name="sniffy_arbgen_set_offset",
            description="Set signal DC offset (V) per channel.",
            inputSchema={
                "type": "object",
                "properties": {
                    "module": {"type": "string"},
                    "channel": {"type": "integer"},
                    "value": {"type": "number", "description": "Offset in volts."},
                },
                "required": ["module", "channel", "value"],
            },
        ),
        Tool(
            name="sniffy_arbgen_set_duty",
            description="Set duty cycle (%) per channel (affects Rect/Saw shapes).",
            inputSchema={
                "type": "object",
                "properties": {
                    "module": {"type": "string"},
                    "channel": {"type": "integer"},
                    "value": {"type": "number", "description": "Duty cycle 0-100%."},
                },
                "required": ["module", "channel", "value"],
            },
        ),
        Tool(
            name="sniffy_arbgen_set_phase",
            description="Set signal phase (degrees 0-360) per channel.",
            inputSchema={
                "type": "object",
                "properties": {
                    "module": {"type": "string"},
                    "channel": {"type": "integer"},
                    "value": {"type": "number", "description": "Phase in degrees."},
                },
                "required": ["module", "channel", "value"],
            },
        ),
        Tool(
            name="sniffy_arbgen_set_channels",
            description="Set number of active generator channels (1-4).",
            inputSchema={
                "type": "object",
                "properties": {
                    "module": {"type": "string"},
                    "count": {"type": "integer", "description": "Number of channels (1-4)."},
                },
                "required": ["module", "count"],
            },
        ),
        Tool(
            name="sniffy_arbgen_set_memory",
            description="Set memory mode: 0=Best fit, 1=Long, 2=Custom. When mode=2, optionally set 'length' (samples).",
            inputSchema={
                "type": "object",
                "properties": {
                    "module": {"type": "string"},
                    "mode":   {"type": "integer", "description": "0=Best fit, 1=Long, 2=Custom."},
                    "length": {"type": "integer", "description": "Custom sample count (only for mode=2)."},
                },
                "required": ["module", "mode"],
            },
        ),
        Tool(
            name="sniffy_arbgen_set_sweep",
            description="Enable/disable SW frequency sweep on CH1.  When enabling, optionally set min_freq (Hz), max_freq (Hz), sweep_time (s).",
            inputSchema={
                "type": "object",
                "properties": {
                    "module":     {"type": "string"},
                    "enable":     {"type": "boolean"},
                    "min_freq":   {"type": "number", "description": "Min sweep frequency (Hz)."},
                    "max_freq":   {"type": "number", "description": "Max sweep frequency (Hz)."},
                    "sweep_time": {"type": "number", "description": "Sweep time (seconds)."},
                },
                "required": ["module", "enable"],
            },
        ),
        Tool(
            name="sniffy_arbgen_set_freq_sync",
            description="Set CH1 frequency sync on/off for a channel. Channel is 0-based index (1=CH2, 2=CH3, 3=CH4).",
            inputSchema={
                "type": "object",
                "properties": {
                    "module":  {"type": "string"},
                    "channel": {"type": "integer", "description": "Channel index 1-3 (CH2-CH4)."},
                    "enabled": {"type": "boolean"},
                },
                "required": ["module", "channel", "enabled"],
            },
        ),
        Tool(
            name="sniffy_arbgen_set_pwm_frequency",
            description="Set PWM carrier frequency per channel (PWM generator only).",
            inputSchema={
                "type": "object",
                "properties": {
                    "module":  {"type": "string", "description": "'PWM generator'."},
                    "channel": {"type": "integer", "description": "Channel index (0-based)."},
                    "value":   {"type": "number", "description": "PWM carrier frequency in Hz."},
                },
                "required": ["module", "channel", "value"],
            },
        ),
        Tool(
            name="sniffy_arbgen_set_pwm_freq_sync",
            description="Set PWM frequency sync with CH1 on/off for a channel (PWM generator only). Channel 1=CH2, 2=CH3, 3=CH4.",
            inputSchema={
                "type": "object",
                "properties": {
                    "module":  {"type": "string", "description": "'PWM generator'."},
                    "channel": {"type": "integer", "description": "Channel index 1-3 (CH2-CH4)."},
                    "enabled": {"type": "boolean"},
                },
                "required": ["module", "channel", "enabled"],
            },
        ),
        # Pattern Generator
        Tool(
            name="sniffy_patgen_set_pattern",
            description=(
                "Select pattern type by index. "
                "0=User defined, 1=Counter, 2=Binary, 3=Gray, 4=Quadrature, "
                "5=PRBS, 6=PWM, 7=Line code, 8=4B/5B, 9=Johnson, 10=PDM, "
                "11=Parallel bus, 12=UART, 13=SPI, 14=I2C."
            ),
            inputSchema={
                "type": "object",
                "properties": {
                    "module": {"type": "string", "description": "'Pattern generator'."},
                    "index": {"type": "integer", "description": "Pattern type index (0-14)."},
                },
                "required": ["module", "index"],
            },
        ),
        Tool(
            name="sniffy_patgen_set_frequency",
            description="Set frequency (Hz) of the currently selected pattern.",
            inputSchema={
                "type": "object",
                "properties": {
                    "module": {"type": "string"},
                    "value": {"type": "number", "description": "Frequency in Hz."},
                },
                "required": ["module", "value"],
            },
        ),
        Tool(
            name="sniffy_patgen_set_channels",
            description=(
                "Set channel count / data length for the current pattern. "
                "Only patterns with a count dial: 0=User defined, 1=Counter, "
                "2=Binary, 3=Gray, 8=4B/5B, 9=Johnson, 11=Parallel bus."
            ),
            inputSchema={
                "type": "object",
                "properties": {
                    "module": {"type": "string", "description": "'Pattern generator'."},
                    "value": {"type": "number", "description": "Channel count / data length."},
                },
                "required": ["module", "value"],
            },
        ),
        Tool(
            name="sniffy_patgen_reset",
            description="Reset the currently selected pattern to factory defaults.",
            inputSchema={
                "type": "object",
                "properties": {
                    "module": {"type": "string", "description": "'Pattern generator'."},
                },
                "required": ["module"],
            },
        ),
        # Voltmeter
        Tool(
            name="sniffy_voltmeter_set_mode",
            description="Set Voltmeter speed mode: 0=Normal, 1=Fast.",
            inputSchema={
                "type": "object",
                "properties": {
                    "module": {"type": "string", "description": "'Voltmeter'."},
                    "mode": {"type": "integer", "description": "0=Normal, 1=Fast."},
                },
                "required": ["module", "mode"],
            },
        ),
        Tool(
            name="sniffy_voltmeter_set_averaging",
            description="Set Voltmeter averaging sample count (1-64).",
            inputSchema={
                "type": "object",
                "properties": {
                    "module": {"type": "string"},
                    "value": {"type": "number", "description": "Averaging count (1-64)."},
                },
                "required": ["module", "value"],
            },
        ),
        Tool(
            name="sniffy_voltmeter_set_calc_mode",
            description="Set Voltmeter calculation display: 0=Min/Max, 1=Ripple, 2=None.",
            inputSchema={
                "type": "object",
                "properties": {
                    "module": {"type": "string"},
                    "mode": {"type": "integer", "description": "0=Min/Max, 1=Ripple, 2=None."},
                },
                "required": ["module", "mode"],
            },
        ),
        Tool(
            name="sniffy_voltmeter_set_channels",
            description="Enable Voltmeter channels by bitmask (bit 0=CH1, bit 1=CH2, etc.).",
            inputSchema={
                "type": "object",
                "properties": {
                    "module": {"type": "string", "description": "Module name (e.g. 'Voltmeter')."},
                    "channel_mask": {"type": "integer", "description": "Bitmask of enabled channels."},
                },
                "required": ["module", "channel_mask"],
            },
        ),
        Tool(
            name="sniffy_voltmeter_get_readings",
            description="Read live Voltmeter readings: voltage, min, max, ripple, frequency, percent per channel, plus Vdd.",
            inputSchema={
                "type": "object",
                "properties": {
                    "module": {"type": "string", "description": "Module name (e.g. 'Voltmeter')."},
                },
                "required": ["module"],
            },
        ),
        Tool(
            name="sniffy_voltmeter_set_datalog_file",
            description="Set data log output file path for the Voltmeter. The file will be created/overwritten when logging starts.",
            inputSchema={
                "type": "object",
                "properties": {
                    "module": {"type": "string", "description": "Module name (e.g. 'Voltmeter')."},
                    "path":   {"type": "string", "description": "Absolute path to the .txt or .csv file."},
                },
                "required": ["module", "path"],
            },
        ),
        Tool(
            name="sniffy_voltmeter_start_datalog",
            description="Start Voltmeter data logging to the previously set file.",
            inputSchema={
                "type": "object",
                "properties": {
                    "module": {"type": "string", "description": "Module name (e.g. 'Voltmeter')."},
                },
                "required": ["module"],
            },
        ),
        Tool(
            name="sniffy_voltmeter_stop_datalog",
            description="Stop Voltmeter data logging.",
            inputSchema={
                "type": "object",
                "properties": {
                    "module": {"type": "string", "description": "Module name (e.g. 'Voltmeter')."},
                },
                "required": ["module"],
            },
        ),
    ]


@app.call_tool()
async def call_tool(name: str, arguments: dict) -> list[TextContent]:
    c = _get_client()

    dispatch = {
        "sniffy_ping":              lambda: c.ping(),
        "sniffy_get_status":        lambda: c.get_status(),
        "sniffy_get_system_config": lambda: c.get_system_config(),
        "sniffy_scan_devices":      lambda: c.scan_devices(),
        "sniffy_list_devices":      lambda: c.list_devices(),
        "sniffy_connect_device":    lambda: c.connect_device(
            index=arguments.get("index"), name=arguments.get("name"),
        ),
        "sniffy_disconnect_device": lambda: c.disconnect_device(),
        "sniffy_list_modules":      lambda: c.list_modules(),
        "sniffy_module_start":      lambda: c.module_start(arguments["module"]),
        "sniffy_module_stop":       lambda: c.module_stop(arguments["module"]),
        "sniffy_module_run":        lambda: c.module_run(arguments["module"]),
        "sniffy_module_halt":       lambda: c.module_halt(arguments["module"]),
        "sniffy_get_module_state":  lambda: c.get_module_state(arguments["module"]),
        "sniffy_write_command":     lambda: c.write_command(arguments["command"]),
        "sniffy_write_command_int": lambda: c.write_command_int(
            arguments["prefix"], arguments["feature"], arguments["value"],
        ),
        "sniffy_scope_read_data":   lambda: c.scope_read_data(),
        "sniffy_module_get_config": lambda: c.module_get_config(arguments["module"]),
        "sniffy_list_displays":     lambda: c.list_displays(arguments["module"]),
        "sniffy_display_save_history":  lambda: c.display_save_history(
            arguments["module"], arguments["display"],
        ),
        "sniffy_display_clear_history": lambda: c.display_clear_history(
            arguments["module"], arguments["display"],
        ),
        "sniffy_display_set_history_size": lambda: c.display_set_history_size(
            arguments["module"], arguments["display"], arguments["size"],
        ),
        # GUI-synced RPCs
        "sniffy_spwm_set_frequency": lambda: c.spwm_set_frequency(
            arguments["module"], arguments["channel"], arguments["value"],
        ),
        "sniffy_spwm_set_duty": lambda: c.spwm_set_duty(
            arguments["module"], arguments["channel"], arguments["value"],
        ),
        "sniffy_spwm_set_phase": lambda: c.spwm_set_phase(
            arguments["module"], arguments["channel"], arguments["value"],
        ),
        "sniffy_spwm_set_channel_enable": lambda: c.spwm_set_channel_enable(
            arguments["module"], arguments["channel"], arguments["enabled"],
        ),
        "sniffy_spwm_set_step_mode": lambda: c.spwm_set_step_mode(
            arguments["module"], arguments["step"],
        ),
        "sniffy_spwm_set_equidistant": lambda: c.spwm_set_equidistant(
            arguments["module"], arguments["enabled"],
        ),
        "sniffy_spwm_set_invert": lambda: c.spwm_set_invert(
            arguments["module"], arguments["channel"], arguments["enabled"],
        ),
        "sniffy_counter_set_mode": lambda: c.counter_set_mode(
            arguments["module"], arguments["mode"],
        ),
        "sniffy_counter_set_gate_time": lambda: c.counter_set_gate_time(
            arguments["module"], arguments["index"],
        ),
        "sniffy_scope_set_timebase": lambda: c.scope_set_timebase(
            arguments["module"], arguments["value"],
        ),
        "sniffy_scope_set_trigger_mode": lambda: c.scope_set_trigger_mode(
            arguments["module"], arguments["mode"],
        ),
        "sniffy_scope_set_trigger_edge": lambda: c.scope_set_trigger_edge(
            arguments["module"], arguments["edge"],
        ),
        "sniffy_scope_set_trigger_channel": lambda: c.scope_set_trigger_channel(
            arguments["module"], arguments["channel"],
        ),
        "sniffy_scope_set_channels": lambda: c.scope_set_channels(
            arguments["module"], arguments["channel_mask"],
        ),
        "sniffy_scope_set_pretrigger": lambda: c.scope_set_pretrigger(
            arguments["module"], arguments["percent"],
        ),
        "sniffy_scope_set_trigger_level": lambda: c.scope_set_trigger_level(
            arguments["module"], arguments["percent"],
        ),
        "sniffy_scope_set_resolution": lambda: c.scope_set_resolution(
            arguments["module"], arguments["bits"],
        ),
        "sniffy_scope_set_sampling_freq": lambda: c.scope_set_sampling_freq(
            arguments["module"], arguments["value"],
        ),
        "sniffy_scope_set_data_length": lambda: c.scope_set_data_length(
            arguments["module"], arguments["value"],
        ),
        "sniffy_scope_add_measurement": lambda: c.scope_add_measurement(
            arguments["module"], arguments["type"],
            arguments.get("channel", 0), arguments.get("channel_b", 1),
        ),
        "sniffy_scope_clear_measurements": lambda: c.scope_clear_measurements(
            arguments["module"],
        ),
        "sniffy_scope_get_measurements": lambda: c.scope_get_measurements(
            arguments["module"],
        ),
        "sniffy_vout_set_voltage": lambda: c.vout_set_voltage(
            arguments["module"], arguments["channel"], arguments["voltage"],
        ),
        # Counter extended
        "sniffy_counter_hf_config": lambda: c.counter_hf_config(
            arguments["module"],
            **{k: arguments[k] for k in ("quantity", "error_mode", "averaging", "gate_time") if k in arguments},
        ),
        "sniffy_counter_lf_config": lambda: c.counter_lf_config(
            arguments["module"],
            **{k: arguments[k] for k in ("channel", "quantity", "multiplier", "duty_cycle", "sample_count", "sample_channel") if k in arguments},
        ),
        "sniffy_counter_rat_set_sample_count": lambda: c.counter_rat_set_sample_count(
            arguments["module"], arguments["value"],
        ),
        "sniffy_counter_int_config": lambda: c.counter_int_config(
            arguments["module"],
            **{k: arguments[k] for k in ("event_sequence", "edge_a", "edge_b", "timeout") if k in arguments},
        ),
        # Arb/PWM Generator
        "sniffy_arbgen_set_frequency": lambda: c.arbgen_set_frequency(
            arguments["module"], arguments["channel"], arguments["value"],
        ),
        "sniffy_arbgen_set_shape": lambda: c.arbgen_set_shape(
            arguments["module"], arguments["channel"], arguments["index"],
        ),
        "sniffy_arbgen_set_amplitude": lambda: c.arbgen_set_amplitude(
            arguments["module"], arguments["channel"], arguments["value"],
        ),
        "sniffy_arbgen_set_offset": lambda: c.arbgen_set_offset(
            arguments["module"], arguments["channel"], arguments["value"],
        ),
        "sniffy_arbgen_set_duty": lambda: c.arbgen_set_duty(
            arguments["module"], arguments["channel"], arguments["value"],
        ),
        "sniffy_arbgen_set_phase": lambda: c.arbgen_set_phase(
            arguments["module"], arguments["channel"], arguments["value"],
        ),
        "sniffy_arbgen_set_channels": lambda: c.arbgen_set_channels(
            arguments["module"], arguments["count"],
        ),
        "sniffy_arbgen_set_memory": lambda: c.arbgen_set_memory(
            arguments["module"], arguments["mode"],
            arguments.get("length", 0),
        ),
        "sniffy_arbgen_set_sweep": lambda: c.arbgen_set_sweep(
            arguments["module"], arguments["enable"],
            arguments.get("min_freq", 0),
            arguments.get("max_freq", 0),
            arguments.get("sweep_time", 0),
        ),
        "sniffy_arbgen_set_freq_sync": lambda: c.arbgen_set_freq_sync(
            arguments["module"], arguments["channel"], arguments["enabled"],
        ),
        "sniffy_arbgen_set_pwm_frequency": lambda: c.arbgen_set_pwm_frequency(
            arguments["module"], arguments["channel"], arguments["value"],
        ),
        "sniffy_arbgen_set_pwm_freq_sync": lambda: c.arbgen_set_pwm_freq_sync(
            arguments["module"], arguments["channel"], arguments["enabled"],
        ),
        # Pattern Generator
        "sniffy_patgen_set_pattern": lambda: c.patgen_set_pattern(
            arguments["module"], arguments["index"],
        ),
        "sniffy_patgen_set_frequency": lambda: c.patgen_set_frequency(
            arguments["module"], arguments["value"],
        ),
        "sniffy_patgen_set_channels": lambda: c.patgen_set_channels(
            arguments["module"], arguments["value"],
        ),
        "sniffy_patgen_reset": lambda: c.patgen_reset(
            arguments["module"],
        ),
        # Voltmeter
        "sniffy_voltmeter_set_mode": lambda: c.voltmeter_set_mode(
            arguments["module"], arguments["mode"],
        ),
        "sniffy_voltmeter_set_averaging": lambda: c.voltmeter_set_averaging(
            arguments["module"], arguments["value"],
        ),
        "sniffy_voltmeter_set_calc_mode": lambda: c.voltmeter_set_calc_mode(
            arguments["module"], arguments["mode"],
        ),
        "sniffy_voltmeter_set_channels": lambda: c.voltmeter_set_channels(
            arguments["module"], arguments["channel_mask"],
        ),
        "sniffy_voltmeter_get_readings": lambda: c.voltmeter_get_readings(
            arguments["module"],
        ),
        "sniffy_voltmeter_set_datalog_file": lambda: c.voltmeter_set_datalog_file(
            arguments["module"], arguments["path"],
        ),
        "sniffy_voltmeter_start_datalog": lambda: c.voltmeter_start_datalog(
            arguments["module"],
        ),
        "sniffy_voltmeter_stop_datalog": lambda: c.voltmeter_stop_datalog(
            arguments["module"],
        ),
    }

    handler = dispatch.get(name)
    if handler is None:
        return [TextContent(type="text", text=f"Unknown tool: {name}")]

    try:
        result = handler()
    except Exception as exc:
        return [TextContent(type="text", text=f"Error: {exc}")]

    return [TextContent(type="text", text=json.dumps(result, indent=2))]


# ── Entry point ──────────────────────────────────────────

def main():
    import asyncio
    asyncio.run(run_server(app))
