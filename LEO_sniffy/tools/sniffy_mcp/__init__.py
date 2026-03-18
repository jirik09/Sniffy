"""sniffy-mcp  –  Python client for the Sniffy AgentBridge IPC server.

Minimal dependency footprint: only the Python stdlib is required for the
core client.  Optional MCP (Model Context Protocol) server wrapper uses
the ``mcp`` package.
"""

__version__ = "0.1.0"

from .client import SniffyClient

__all__ = ["SniffyClient"]
