#!/usr/bin/env python3
"""Helpers for sniffy-mcp release automation."""

from __future__ import annotations

import argparse
import json
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parent
VERSIONS_FILE_PARTS = ("tools", "release", "versions.toml")
PYPROJECT = ROOT / "pyproject.toml"
PACKAGE_DIR = ROOT / "sniffy_mcp"
PACKAGE_INIT = PACKAGE_DIR / "__init__.py"
TAG_PREFIX = "mcp-v"
VERSION_PATTERN = re.compile(r"^(\d+)\.(\d+)\.(\d+)$")
TAG_PATTERN = re.compile(rf"^{TAG_PREFIX}(\d+\.\d+\.\d+)$")


def find_versions_file() -> Path:
    for candidate_root in (ROOT, *ROOT.parents):
        candidate = candidate_root.joinpath(*VERSIONS_FILE_PARTS)
        if candidate.exists():
            return candidate
    raise SystemExit(
        f"Could not find {'/'.join(VERSIONS_FILE_PARTS)} from {ROOT}"
    )


def read_versions_file() -> dict[str, str]:
    versions_path = find_versions_file()
    versions: dict[str, str] = {}
    current_section: str | None = None

    for raw_line in versions_path.read_text(encoding="utf-8").splitlines():
        line = raw_line.split("#", 1)[0].strip()
        if not line:
            continue

        section_match = re.fullmatch(r"\[([A-Za-z0-9_-]+)\]", line)
        if section_match is not None:
            current_section = section_match.group(1)
            continue

        version_match = re.fullmatch(r'version\s*=\s*"([^"]+)"', line)
        if version_match is not None and current_section is not None:
            versions[current_section] = version_match.group(1)

    return versions


def read_declared_version() -> str:
    versions = read_versions_file()
    version = versions.get("mcp")
    if version is None:
        raise SystemExit(
            f"Missing [mcp].version entry in {find_versions_file()}"
        )

    validate_version(version)
    return version


def load_metadata(version: str | None = None) -> dict[str, str]:
    resolved_version = version or read_declared_version()
    validate_version(resolved_version)

    return {
        "package": "sniffy-mcp",
        "version": resolved_version,
        "tag": f"{TAG_PREFIX}{resolved_version}",
        "wheel": f"sniffy_mcp-{resolved_version}-py3-none-any.whl",
        "sdist": f"sniffy_mcp-{resolved_version}.tar.gz",
    }


def validate_version(version: str) -> None:
    if VERSION_PATTERN.fullmatch(version) is None:
        raise SystemExit(f"Unsupported version format: {version}")


def version_from_tag(tag: str) -> str:
    match = TAG_PATTERN.fullmatch(tag)
    if match is None:
        raise SystemExit(
            f"Unsupported MCP tag format: {tag}. Expected {TAG_PREFIX}X.Y.Z"
        )
    return match.group(1)


def resolve_version(
    *,
    version: str | None = None,
    tag: str | None = None,
) -> str:
    if version is not None:
        validate_version(version)
        return version
    if tag is not None:
        return version_from_tag(tag)
    return read_declared_version()


def replace_exactly_once(text: str, pattern: str, replacement: str, label: str) -> str:
    updated_text, count = re.subn(pattern, replacement, text, count=1, flags=re.MULTILINE)
    if count != 1:
        raise SystemExit(f"Could not update version in {label}")
    return updated_text


def write_version(version: str) -> None:
    validate_version(version)

    pyproject_text = PYPROJECT.read_text(encoding="utf-8")
    pyproject_text = replace_exactly_once(
        pyproject_text,
        r'^version\s*=\s*"[^"]+"$',
        f'version = "{version}"',
        "pyproject.toml",
    )
    PYPROJECT.write_text(pyproject_text, encoding="utf-8")

    init_text = PACKAGE_INIT.read_text(encoding="utf-8")
    init_text = replace_exactly_once(
        init_text,
        r'^__version__\s*=\s*(?:_read_repo_version\(\)\s+or\s+)?"[^"]+"$',
        f'__version__ = _read_repo_version() or "{version}"',
        "sniffy_mcp/__init__.py",
    )
    PACKAGE_INIT.write_text(init_text, encoding="utf-8")

def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    subparsers = parser.add_subparsers(dest="command", required=True)

    show_parser = subparsers.add_parser("show", help="Print package release metadata")
    show_parser.add_argument(
        "--tag",
        help="Resolve metadata from an explicit MCP tag such as mcp-v0.1.0",
    )
    show_parser.add_argument(
        "--field",
        choices=["package", "version", "tag", "wheel", "sdist"],
        help="Print just one field",
    )

    validate_parser = subparsers.add_parser("validate", help="Validate release metadata")
    validate_parser.add_argument(
        "--tag",
        help="Resolve metadata from an explicit MCP tag such as mcp-v0.1.0",
    )
    validate_parser.add_argument(
        "--expected-tag",
        help="Fail if the computed MCP tag differs from this value",
    )

    write_parser = subparsers.add_parser(
        "write-version",
        help="Write a resolved version into pyproject.toml and sniffy_mcp/__init__.py",
    )
    version_group = write_parser.add_mutually_exclusive_group(required=True)
    version_group.add_argument(
        "--version",
        help="Explicit version to write",
    )
    version_group.add_argument(
        "--tag",
        help="Resolve the version from an MCP tag such as mcp-v0.1.0",
    )

    args = parser.parse_args()

    if args.command == "show":
        metadata = load_metadata(resolve_version(tag=args.tag))
        if args.field:
            print(metadata[args.field])
        else:
            print(json.dumps(metadata, indent=2, ensure_ascii=True))
        return

    if args.command == "validate":
        metadata = load_metadata(resolve_version(tag=args.tag))
        if args.expected_tag and metadata["tag"] != args.expected_tag:
            raise SystemExit(
                f"Expected MCP tag {args.expected_tag}, got {metadata['tag']}"
            )
        print(metadata["tag"])
        return

    if args.command == "write-version":
        version = resolve_version(
            version=args.version,
            tag=args.tag,
        )
        write_version(version)
        print(version)
        return

    raise SystemExit("Unknown command")


if __name__ == "__main__":
    main()