"""Package entry point — ``python -m sniffy_mcp``.

Modes
-----
MCP server (default, no args):
    python -m sniffy_mcp

Quick CLI call:
    python -m sniffy_mcp call <method> [arg1] [arg2] ...

Examples:
    python -m sniffy_mcp call get_status
    python -m sniffy_mcp call module_start "Sync PWM"
    python -m sniffy_mcp call display_set_history_size Counter HighFreqCounter 555
    python -m sniffy_mcp call spwm_set_frequency "Sync PWM" 0 5000
"""

from __future__ import annotations

import json
import sys


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

        # Coerce arguments: try int, then float, then keep as str
        coerced: list = []
        for a in raw_args:
            try:
                coerced.append(int(a))
            except ValueError:
                try:
                    coerced.append(float(a))
                except ValueError:
                    coerced.append(a)

        result = fn(*coerced)
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
