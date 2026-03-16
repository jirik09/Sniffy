"""Low-level client for the Sniffy AgentBridge JSON-RPC server.

Connects to the ``sniffy-agent`` named pipe (or TCP fallback) and
exposes every JSON-RPC method as a typed Python method.

Usage::

    from sniffy_mcp import SniffyClient

    with SniffyClient() as s:
        status = s.get_status()
        print(status["device"], status["modules"])

        s.module_start("Oscilloscope")
        s.write_command("OSCP:TRIG:AUTO")
        data = s.scope_read_data()
        s.module_stop("Oscilloscope")
"""

from __future__ import annotations

import json
import struct
import socket
import sys
import time
from pathlib import Path
from typing import Any

# On Windows, named pipes require the win32file API or raw CreateFile.
# QLocalServer creates \\.\pipe\<name> — we use Python's socket on
# Unix and ctypes on Windows for zero-dependency connectivity.
_IS_WIN = sys.platform == "win32"

if _IS_WIN:
    import ctypes
    import ctypes.wintypes as wt

    _GENERIC_READ      = 0x80000000
    _GENERIC_WRITE     = 0x40000000
    _OPEN_EXISTING     = 3
    _INVALID_HANDLE    = wt.HANDLE(-1).value & 0xFFFFFFFF  # type: ignore[arg-type]
    _PIPE_READMODE_BYTE = 0x00000000

    _kernel32 = ctypes.WinDLL("kernel32", use_last_error=True)

    def _CreateFileW(name, access, share, sec, disp, flags, template):
        h = _kernel32.CreateFileW(name, access, share, sec, disp, flags, template)
        return h

    def _ReadFile(handle, size):
        buf = ctypes.create_string_buffer(size)
        read = wt.DWORD(0)
        ok = _kernel32.ReadFile(handle, buf, size, ctypes.byref(read), None)
        if not ok:
            raise OSError(f"ReadFile failed (err={ctypes.get_last_error()})")
        return buf.raw[: read.value]

    def _WriteFile(handle, data: bytes):
        written = wt.DWORD(0)
        ok = _kernel32.WriteFile(handle, data, len(data), ctypes.byref(written), None)
        if not ok:
            raise OSError(f"WriteFile failed (err={ctypes.get_last_error()})")
        return written.value

    def _CloseHandle(handle):
        _kernel32.CloseHandle(handle)


class SniffyClient:
    """Synchronous client for the Sniffy Agent Bridge.

    Parameters
    ----------
    pipe_name : str
        Named-pipe / Unix-socket name that the Qt app is listening on.
    timeout : float
        Read timeout in seconds.
    """

    def __init__(self, pipe_name: str = "sniffy-agent", timeout: float = 5.0):
        self._pipe_name = pipe_name
        self._timeout = timeout
        self._handle: Any = None  # Win32 handle
        self._sock: socket.socket | None = None  # Unix socket
        self._id_counter = 0

    # ── connection management ──────────────────────────────

    def connect(self) -> None:
        """Open the named-pipe / Unix-socket connection."""
        if _IS_WIN:
            path = rf"\\.\pipe\{self._pipe_name}"
            h = _CreateFileW(
                path,
                _GENERIC_READ | _GENERIC_WRITE,
                0, None,
                _OPEN_EXISTING, 0, None,
            )
            if (h & 0xFFFFFFFF) == _INVALID_HANDLE:
                raise ConnectionError(
                    f"Cannot connect to Sniffy pipe '{path}' "
                    f"(err={ctypes.get_last_error()}).  Is the app running?"
                )
            self._handle = h
        else:
            # Unix: QLocalServer puts the socket under /tmp or XDG_RUNTIME_DIR
            candidates = [
                Path(f"/tmp/{self._pipe_name}"),
                Path(f"/run/user/{__import__('os').getuid()}/{self._pipe_name}"),
            ]
            for p in candidates:
                if p.exists():
                    s = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
                    s.settimeout(self._timeout)
                    s.connect(str(p))
                    self._sock = s
                    return
            raise ConnectionError(
                f"Cannot find Sniffy socket ({', '.join(str(p) for p in candidates)}).  "
                "Is the app running?"
            )

    def close(self) -> None:
        """Close the connection."""
        if _IS_WIN and self._handle is not None:
            _CloseHandle(self._handle)
            self._handle = None
        if self._sock is not None:
            self._sock.close()
            self._sock = None

    def __enter__(self):
        self.connect()
        return self

    def __exit__(self, *exc):
        self.close()

    # ── low-level transport ────────────────────────────────

    def _send(self, data: bytes) -> None:
        header = struct.pack(">I", len(data))
        if _IS_WIN:
            _WriteFile(self._handle, header + data)
        else:
            assert self._sock is not None
            self._sock.sendall(header + data)

    def _recv_exact(self, n: int) -> bytes:
        buf = b""
        deadline = time.monotonic() + self._timeout
        while len(buf) < n:
            remaining = n - len(buf)
            if _IS_WIN:
                chunk = _ReadFile(self._handle, remaining)
            else:
                assert self._sock is not None
                chunk = self._sock.recv(remaining)
            if not chunk:
                raise ConnectionError("Pipe closed unexpectedly")
            buf += chunk
            if time.monotonic() > deadline:
                raise TimeoutError("Read timeout")
        return buf

    def _recv(self) -> bytes:
        header = self._recv_exact(4)
        (length,) = struct.unpack(">I", header)
        if length > 16 * 1024 * 1024:
            raise ValueError(f"Response too large ({length} bytes)")
        return self._recv_exact(length)

    # ── JSON-RPC call ──────────────────────────────────────

    def _call(self, method: str, params: dict | None = None) -> dict:
        self._id_counter += 1
        req = {"jsonrpc": "2.0", "id": self._id_counter, "method": method}
        if params:
            req["params"] = params
        self._send(json.dumps(req).encode())
        raw = self._recv()
        resp = json.loads(raw)
        if "error" in resp:
            err = resp["error"]
            raise RuntimeError(f"RPC error {err.get('code')}: {err.get('message')}")
        return resp.get("result", {})

    # ── high-level API ─────────────────────────────────────

    def ping(self) -> dict:
        """Health-check."""
        return self._call("ping")

    def get_status(self) -> dict:
        """Device connection status and module list."""
        return self._call("get_status")

    def get_system_config(self) -> dict:
        """Full hardware configuration: device info, per-module capabilities,
        channel counts, pins, and resource conflict matrix."""
        return self._call("get_system_config")

    # ── Device management ──────────────────────────────────

    def scan_devices(self) -> dict:
        """Probe all COM ports for Sniffy-compatible boards. Returns list of found devices."""
        return self._call("scan_devices")

    def list_devices(self) -> dict:
        """Return the last scanned device list (no re-scan)."""
        return self._call("list_devices")

    def connect_device(self, index: int | None = None, name: str | None = None) -> dict:
        """Connect to a device by index or name substring (e.g. 'F303RE')."""
        params: dict[str, Any] = {}
        if index is not None:
            params["index"] = index
        elif name is not None:
            params["name"] = name
        return self._call("connect_device", params)

    def disconnect_device(self) -> dict:
        """Disconnect the current device."""
        return self._call("disconnect_device")

    # ── Module lifecycle ───────────────────────────────────

    def list_modules(self) -> list[dict]:
        """List all modules with their current state."""
        return self._call("list_modules").get("modules", [])

    def module_start(self, name: str) -> dict:
        """Start a module (same as clicking its button in the GUI)."""
        return self._call("module_start", {"module": name})

    def module_stop(self, name: str) -> dict:
        """Stop a module."""
        return self._call("module_stop", {"module": name})

    def get_module_state(self, name: str) -> dict:
        """Read current GUI state of a running module."""
        return self._call("get_module_state", {"module": name})

    def module_get_config(self, name: str) -> dict:
        """Get a module's serialized configuration (base64)."""
        return self._call("module_get_config", {"module": name})

    def write_command(self, command: str) -> dict:
        """Send a raw ASCII protocol command (e.g. ``"OSCP:TRIG:AUTO"``)."""
        return self._call("write_command", {"command": command})

    def write_command_int(self, prefix: str, feature: str, value: int) -> dict:
        """Send a MODULE:FEATURE with 32-bit integer parameter."""
        return self._call("write_command_int", {
            "prefix": prefix, "feature": feature, "value": value,
        })

    def scope_read_data(self) -> dict:
        """Return the last captured scope snapshot (JSON arrays)."""
        return self._call("scope_read_data")

    def scope_read_data_compact(self) -> dict:
        """Return the last scope snapshot with base64-packed float data."""
        return self._call("scope_read_data_compact")

    def list_displays(self, module: str) -> list[str]:
        """List WidgetDisplay names inside a module (e.g. 'LowFreqCh1Counter')."""
        return self._call("list_displays", {"module": module}).get("displays", [])

    def display_save_history(self, module: str, display: str) -> dict:
        """Save a display's measurement history to a text file."""
        return self._call("display_save_history", {"module": module, "display": display})

    def display_clear_history(self, module: str, display: str) -> dict:
        """Clear a display's measurement history."""
        return self._call("display_clear_history", {"module": module, "display": display})

    def display_set_history_size(self, module: str, display: str, size: int) -> dict:
        """Set the history sample count (100-1000) for a display."""
        return self._call("display_set_history_size", {"module": module, "display": display, "size": size})

    # ── GUI-synced high-level RPCs ─────────────────────────

    # Sync PWM

    def spwm_set_frequency(self, module: str, channel: int, value: float) -> dict:
        """Set Sync PWM channel frequency (Hz). Updates GUI + MCU."""
        return self._call("spwm_set_frequency", {"module": module, "channel": channel, "value": value})

    def spwm_set_duty(self, module: str, channel: int, value: float) -> dict:
        """Set Sync PWM channel duty cycle (%). Updates GUI + MCU."""
        return self._call("spwm_set_duty", {"module": module, "channel": channel, "value": value})

    def spwm_set_phase(self, module: str, channel: int, value: float) -> dict:
        """Set Sync PWM channel phase (degrees 0-360). Updates GUI + MCU."""
        return self._call("spwm_set_phase", {"module": module, "channel": channel, "value": value})

    def spwm_set_channel_enable(self, module: str, channel: int, enabled: bool = True) -> dict:
        """Enable/disable a Sync PWM channel. Updates GUI + MCU."""
        return self._call("spwm_set_channel_enable", {"module": module, "channel": channel, "enabled": enabled})

    def spwm_set_step_mode(self, module: str, step: bool = True) -> dict:
        """Switch Sync PWM between Continuous (false) and Step (true) mode."""
        return self._call("spwm_set_step_mode", {"module": module, "step": step})

    def spwm_set_equidistant(self, module: str, enabled: bool = True) -> dict:
        """Enable/disable equidistant phase distribution across channels."""
        return self._call("spwm_set_equidistant", {"module": module, "enabled": enabled})

    def spwm_set_invert(self, module: str, channel: int, enabled: bool = True) -> dict:
        """Enable/disable inversion for a specific Sync PWM channel."""
        return self._call("spwm_set_invert", {"module": module, "channel": channel, "enabled": enabled})

    # Counter

    def counter_set_mode(self, module: str, mode: int) -> dict:
        """Switch counter mode (0=HF, 1=LF, 2=Ratio, 3=Intervals). Updates GUI + MCU."""
        return self._call("counter_set_mode", {"module": module, "mode": mode})

    def counter_set_gate_time(self, module: str, index: int) -> dict:
        """Set HF counter gate time (0=100ms,1=500ms,2=1s,3=5s,4=10s). Updates GUI + MCU."""
        return self._call("counter_set_gate_time", {"module": module, "index": index})

    # Scope

    def scope_set_timebase(self, module: str, value: float) -> dict:
        """Set scope time/div in seconds."""
        return self._call("scope_set_timebase", {"module": module, "value": value})

    def scope_set_trigger_mode(self, module: str, mode: str) -> dict:
        """Set scope trigger mode: auto|normal|single|stop."""
        return self._call("scope_set_trigger_mode", {"module": module, "mode": mode})

    def scope_set_trigger_edge(self, module: str, edge: str) -> dict:
        """Set scope trigger edge: rising|falling."""
        return self._call("scope_set_trigger_edge", {"module": module, "edge": edge})

    def scope_set_trigger_channel(self, module: str, channel: int) -> dict:
        """Set scope trigger channel (0-based index)."""
        return self._call("scope_set_trigger_channel", {"module": module, "channel": channel})

    def scope_set_channels(self, module: str, channel_mask: int) -> dict:
        """Enable scope channels by bitmask (bit 0 = CH1, bit 1 = CH2, etc.)."""
        return self._call("scope_set_channels", {"module": module, "channel_mask": channel_mask})

    def scope_set_pretrigger(self, module: str, percent: float) -> dict:
        """Set scope pretrigger percentage (0-100)."""
        return self._call("scope_set_pretrigger", {"module": module, "percent": percent})

    def scope_set_trigger_level(self, module: str, percent: float) -> dict:
        """Set scope trigger level percentage (0-100)."""
        return self._call("scope_set_trigger_level", {"module": module, "percent": percent})

    def scope_set_resolution(self, module: str, bits: int) -> dict:
        """Set scope ADC resolution (8 or 12 bits)."""
        return self._call("scope_set_resolution", {"module": module, "bits": bits})

    def scope_set_sampling_freq(self, module: str, value: int) -> dict:
        """Set scope custom sampling frequency in Hz."""
        return self._call("scope_set_sampling_freq", {"module": module, "value": value})

    def scope_set_data_length(self, module: str, value: int) -> dict:
        """Set scope custom data length in samples."""
        return self._call("scope_set_data_length", {"module": module, "value": value})

    def scope_add_measurement(self, module: str, type: str, channel: int = 0, channel_b: int = 1) -> dict:
        """Add a scope measurement. Types: frequency|period|phase|duty|low|high|rms|rms_ac|mean|pkpk|max|min.
        For phase, channel_b specifies the second channel."""
        params: dict = {"module": module, "type": type, "channel": channel}
        if type.lower() == "phase":
            params["channel_b"] = channel_b
        return self._call("scope_add_measurement", params)

    def scope_clear_measurements(self, module: str) -> dict:
        """Clear all scope measurements."""
        return self._call("scope_clear_measurements", {"module": module})

    def scope_get_measurements(self, module: str) -> dict:
        """Get current scope measurement results."""
        return self._call("scope_get_measurements", {"module": module})

    # Voltage Source

    def vout_set_voltage(self, module: str, channel: int, voltage: float) -> dict:
        """Set DAC output voltage for a channel. Updates GUI + MCU."""
        return self._call("vout_set_voltage", {"module": module, "channel": channel, "voltage": voltage})

    # Counter — additional tab configs

    def counter_hf_config(self, module: str, **kwargs) -> dict:
        """Configure HF counter tab. Optional keys: quantity (0=Freq,1=Period),
        error_mode (0=Err,1=ErrAVG), averaging (samples), gate_time (0-4)."""
        return self._call("counter_hf_config", {"module": module, **kwargs})

    def counter_lf_config(self, module: str, **kwargs) -> dict:
        """Configure LF counter tab. Optional keys: channel (0=CH1,1=CH2),
        quantity (0=Freq,1=Period), multiplier (0-3: 1x/2x/4x/8x),
        duty_cycle (0=Disable,1=Enable), sample_count, sample_channel (0/1)."""
        return self._call("counter_lf_config", {"module": module, **kwargs})

    def counter_rat_set_sample_count(self, module: str, value: float) -> dict:
        """Set Ratio counter sample count dial."""
        return self._call("counter_rat_set_sample_count", {"module": module, "value": value})

    def counter_int_config(self, module: str, **kwargs) -> dict:
        """Configure Intervals counter. Optional keys: event_sequence (0=A→B,1=B→A),
        edge_a ('rising'/'falling'), edge_b ('rising'/'falling'), timeout (seconds)."""
        return self._call("counter_int_config", {"module": module, **kwargs})

    # Arbitrary / PWM Generator

    def arbgen_set_frequency(self, module: str, channel: int, value: float) -> dict:
        """Set Arb/PWM generator frequency (Hz) per channel."""
        return self._call("arbgen_set_frequency", {"module": module, "channel": channel, "value": value})

    def arbgen_set_shape(self, module: str, channel: int, index: int) -> dict:
        """Set signal shape: 0=Sine, 1=Saw, 2=Rect, 3=Arb."""
        return self._call("arbgen_set_shape", {"module": module, "channel": channel, "index": index})

    def arbgen_set_amplitude(self, module: str, channel: int, value: float) -> dict:
        """Set signal amplitude (V) per channel."""
        return self._call("arbgen_set_amplitude", {"module": module, "channel": channel, "value": value})

    def arbgen_set_offset(self, module: str, channel: int, value: float) -> dict:
        """Set signal offset (V) per channel."""
        return self._call("arbgen_set_offset", {"module": module, "channel": channel, "value": value})

    def arbgen_set_duty(self, module: str, channel: int, value: float) -> dict:
        """Set duty cycle (%) per channel."""
        return self._call("arbgen_set_duty", {"module": module, "channel": channel, "value": value})

    def arbgen_set_phase(self, module: str, channel: int, value: float) -> dict:
        """Set phase (degrees 0-360) per channel."""
        return self._call("arbgen_set_phase", {"module": module, "channel": channel, "value": value})

    def arbgen_set_channels(self, module: str, count: int) -> dict:
        """Set number of active channels (1-4)."""
        return self._call("arbgen_set_channels", {"module": module, "count": count})

    def arbgen_set_memory(self, module: str, mode: int, length: int = 0) -> dict:
        """Set memory mode: 0=Best fit, 1=Long, 2=Custom.
        When mode=2 (Custom), length sets the sample count."""
        params: dict = {"module": module, "mode": mode}
        if mode == 2 and length > 0:
            params["length"] = length
        return self._call("arbgen_set_memory", params)

    def arbgen_set_sweep(self, module: str, enable: bool,
                         min_freq: float = 0, max_freq: float = 0,
                         sweep_time: float = 0) -> dict:
        """Enable/disable SW sweep on CH1.  When enabling, optionally set
        min_freq (Hz), max_freq (Hz), sweep_time (s)."""
        params: dict = {"module": module, "enable": enable}
        if enable:
            if min_freq > 0:
                params["min_freq"] = min_freq
            if max_freq > 0:
                params["max_freq"] = max_freq
            if sweep_time > 0:
                params["sweep_time"] = sweep_time
        return self._call("arbgen_set_sweep", params)

    def arbgen_set_freq_sync(self, module: str, channel: int, enabled: bool) -> dict:
        """Set CH1 freq sync on/off for channel (1-3 = CH2-CH4)."""
        return self._call("arbgen_set_freq_sync", {"module": module, "channel": channel, "enabled": enabled})

    def arbgen_set_pwm_frequency(self, module: str, channel: int, value: float) -> dict:
        """Set PWM carrier frequency per channel (PWM generator only)."""
        return self._call("arbgen_set_pwm_frequency", {"module": module, "channel": channel, "value": value})

    def arbgen_set_pwm_freq_sync(self, module: str, channel: int, enabled: bool) -> dict:
        """Set PWM freq sync with CH1 on/off for channel (1-3 = CH2-CH4). PWM generator only."""
        return self._call("arbgen_set_pwm_freq_sync", {"module": module, "channel": channel, "enabled": enabled})

    # Pattern Generator

    def patgen_set_pattern(self, module: str, index: int) -> dict:
        """Select pattern type (0-14). See AGENT_INSTRUCTIONS for pattern names."""
        return self._call("patgen_set_pattern", {"module": module, "index": index})

    def patgen_set_frequency(self, module: str, value: float) -> dict:
        """Set frequency (Hz) of the currently selected pattern."""
        return self._call("patgen_set_frequency", {"module": module, "value": value})

    def patgen_set_channels(self, module: str, value: float) -> dict:
        """Set channel count / data length for the current pattern (patterns 0-3, 8, 9, 11 only)."""
        return self._call("patgen_set_channels", {"module": module, "value": value})

    def patgen_reset(self, module: str) -> dict:
        """Reset the current pattern to factory defaults."""
        return self._call("patgen_reset", {"module": module})

    # Voltmeter

    def voltmeter_set_mode(self, module: str, mode: int) -> dict:
        """Set voltmeter speed mode: 0=Normal, 1=Fast."""
        return self._call("voltmeter_set_mode", {"module": module, "mode": mode})

    def voltmeter_set_averaging(self, module: str, value: float) -> dict:
        """Set voltmeter averaging sample count (1-64)."""
        return self._call("voltmeter_set_averaging", {"module": module, "value": value})

    def voltmeter_set_calc_mode(self, module: str, mode: int) -> dict:
        """Set voltmeter calculation display: 0=Min/Max, 1=Ripple, 2=None."""
        return self._call("voltmeter_set_calc_mode", {"module": module, "mode": mode})

    def voltmeter_set_channels(self, module: str, channel_mask: int) -> dict:
        """Enable voltmeter channels by bitmask (bit 0 = CH1, etc.)."""
        return self._call("voltmeter_set_channels", {"module": module, "channel_mask": channel_mask})

    def voltmeter_get_readings(self, module: str) -> dict:
        """Get live voltmeter readings (voltage, min, max, ripple, frequency per channel)."""
        return self._call("voltmeter_get_readings", {"module": module})

    def voltmeter_set_datalog_file(self, module: str, path: str) -> dict:
        """Set the data log output file path (must exist or be creatable)."""
        return self._call("voltmeter_set_datalog_file", {"module": module, "path": path})

    def voltmeter_start_datalog(self, module: str) -> dict:
        """Start data logging (file must be set first)."""
        return self._call("voltmeter_start_datalog", {"module": module})

    def voltmeter_stop_datalog(self, module: str) -> dict:
        """Stop data logging."""
        return self._call("voltmeter_stop_datalog", {"module": module})

    # ── convenience helpers ────────────────────────────────

    def wait_for_connection(self, timeout: float = 10.0, poll: float = 0.3) -> bool:
        """Block until the app is reachable or timeout expires."""
        deadline = time.monotonic() + timeout
        while time.monotonic() < deadline:
            try:
                self.connect()
                self.ping()
                return True
            except (ConnectionError, OSError):
                time.sleep(poll)
        return False

    def wait_for_device(self, timeout: float = 15.0, poll: float = 0.5) -> bool:
        """Poll get_status until connected==true or timeout expires.

        Use after connect_device to wait for the MCU to finish
        enumerating modules before calling module_start.
        """
        deadline = time.monotonic() + timeout
        while time.monotonic() < deadline:
            try:
                st = self.get_status()
                if st.get("connected"):
                    return True
            except Exception:
                pass
            time.sleep(poll)
        return False

    def wait_for_scope_data(self, timeout: float = 5.0, poll: float = 0.1) -> dict:
        """Poll until a fresh scope snapshot is available."""
        deadline = time.monotonic() + timeout
        while time.monotonic() < deadline:
            try:
                data = self.scope_read_data()
                if data.get("channels"):
                    return data
            except RuntimeError:
                pass
            time.sleep(poll)
        raise TimeoutError("No scope data received within timeout")
