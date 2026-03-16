"""Package entry point — ``python -m sniffy_mcp``.

Modes
-----
MCP server (default, no args):
    python -m sniffy_mcp

Quick CLI call:
    python -m sniffy_mcp call <method> [arg1] [arg2] ...

Examples:
    python -m sniffy_mcp call get_status
    python -m sniffy_mcp call scan_devices
    python -m sniffy_mcp call connect_device 0           # by index
    python -m sniffy_mcp call connect_device "F303RE"    # by name substring
    python -m sniffy_mcp call module_start "Sync PWM"
    python -m sniffy_mcp call display_set_history_size Counter HighFreqCounter 555
    python -m sniffy_mcp call spwm_set_frequency "Sync PWM" 0 5000
"""

from __future__ import annotations

import json
import sys


def _coerce(a: str):
    """Try int, then float, then keep as str."""
    try:
        return int(a)
    except ValueError:
        try:
            return float(a)
        except ValueError:
            return a


def _cli_call(args: list[str]) -> None:
    """Call a single SniffyClient method from the command line."""
    if not args:
        print("Usage: python -m sniffy_mcp call <method> [args...]", file=sys.stderr)
        sys.exit(1)

    from .client import SniffyClient

    method_name = args[0]
    raw_args = args[1:]

    c = SniffyClient()
    c.connect()
    try:
        fn = getattr(c, method_name, None)
        if fn is None or method_name.startswith("_"):
            print(f"Unknown method: {method_name}", file=sys.stderr)
            sys.exit(1)

        # Split args into positional and key=value pairs
        positional_raw = [a for a in raw_args if "=" not in a]
        kw_raw = [a for a in raw_args if "=" in a]
        coerced = [_coerce(a) for a in positional_raw]
        kwargs = {k: _coerce(v) for k, v in (a.split("=", 1) for a in kw_raw)}

        # connect_device takes keyword args: index (int) or name (str)
        if method_name == "connect_device" and len(coerced) == 1:
            arg = coerced[0]
            if isinstance(arg, int):
                result = fn(index=arg)
            else:
                result = fn(name=str(arg))
        else:
            result = fn(*coerced, **kwargs)

        print(json.dumps(result, indent=2))
    finally:
        c.close()


def main() -> None:
    if len(sys.argv) >= 2 and sys.argv[1] == "call":
        _cli_call(sys.argv[2:])
    else:
        # Default: start MCP server (requires 'mcp' package)
        from .mcp_server import main as mcp_main
        mcp_main()


if __name__ == "__main__":
    main()
