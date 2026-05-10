"""sniffy-mcp  –  Python client for the Sniffy AgentBridge IPC server.

Minimal dependency footprint: only the Python stdlib is required for the
core client.  Optional MCP (Model Context Protocol) server wrapper uses
the ``mcp`` package.
"""

from pathlib import Path
import re


def _read_repo_version() -> str | None:
	version_pattern = re.compile(r'^version\s*=\s*"([^"]+)"$')
	section_pattern = re.compile(r'^\[([A-Za-z0-9_-]+)\]$')
	current_section = None

	for parent in (Path(__file__).resolve().parent, *Path(__file__).resolve().parents):
		versions_path = parent / "tools" / "release" / "versions.toml"
		if not versions_path.exists():
			continue

		for raw_line in versions_path.read_text(encoding="utf-8").splitlines():
			line = raw_line.split("#", 1)[0].strip()
			if not line:
				continue

			section_match = section_pattern.fullmatch(line)
			if section_match is not None:
				current_section = section_match.group(1)
				continue

			version_match = version_pattern.fullmatch(line)
			if version_match is not None and current_section == "mcp":
				return version_match.group(1)

	return None


__version__ = _read_repo_version() or "0.1.0"

from .client import SniffyClient

__all__ = ["SniffyClient"]
