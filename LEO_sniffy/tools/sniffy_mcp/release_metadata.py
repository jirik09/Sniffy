#!/usr/bin/env python3
"""Helpers for sniffy-mcp release automation."""

from __future__ import annotations

import argparse
import json
import re
import subprocess
import tomllib
from pathlib import Path

ROOT = Path(__file__).resolve().parent
PYPROJECT = ROOT / "pyproject.toml"
PACKAGE_INIT = ROOT / "__init__.py"
TAG_PREFIX = "mcp-v"
VERSION_PATTERN = re.compile(r"^(\d+)\.(\d+)\.(\d+)$")
TAG_PATTERN = re.compile(rf"^{TAG_PREFIX}(\d+\.\d+\.\d+)$")


def read_declared_version() -> str:
    project = tomllib.loads(PYPROJECT.read_text(encoding="utf-8"))["project"]
    version = project["version"]
    validate_version(version)

    init_text = PACKAGE_INIT.read_text(encoding="utf-8")
    match = re.search(r'^__version__\s*=\s*"([^"]+)"', init_text, re.MULTILINE)
    if match is None:
        raise SystemExit("__init__.py is missing __version__")

    init_version = match.group(1)
    if version != init_version:
        raise SystemExit(
            "Version mismatch between pyproject.toml "
            f"({version}) and __init__.py ({init_version})"
        )

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


def version_key(version: str) -> tuple[int, int, int]:
    match = VERSION_PATTERN.fullmatch(version)
    if match is None:
        raise SystemExit(f"Unsupported version format: {version}")
    return tuple(int(part) for part in match.groups())


def version_from_tag(tag: str) -> str:
    match = TAG_PATTERN.fullmatch(tag)
    if match is None:
        raise SystemExit(
            f"Unsupported MCP tag format: {tag}. Expected {TAG_PREFIX}X.Y.Z"
        )
    return match.group(1)


def bump_patch(version: str) -> str:
    major, minor, patch = version_key(version)
    return f"{major}.{minor}.{patch + 1}"


def latest_mcp_tag() -> str | None:
    result = subprocess.run(
        ["git", "tag", "--list", f"{TAG_PREFIX}*", "--sort=-version:refname"],
        cwd=ROOT,
        capture_output=True,
        text=True,
        check=True,
    )
    for line in result.stdout.splitlines():
        tag = line.strip()
        if tag:
            return tag
    return None


def auto_bumped_version() -> str:
    declared_version = read_declared_version()
    last_tag = latest_mcp_tag()
    if last_tag is None:
        return declared_version

    next_patch_version = bump_patch(version_from_tag(last_tag))
    if version_key(declared_version) >= version_key(next_patch_version):
        return declared_version
    return next_patch_version


def resolve_version(
    *,
    version: str | None = None,
    tag: str | None = None,
    auto_bump: bool = False,
) -> str:
    if version is not None:
        validate_version(version)
        return version
    if tag is not None:
        return version_from_tag(tag)
    if auto_bump:
        return auto_bumped_version()
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
        r'^__version__\s*=\s*"[^"]+"$',
        f'__version__ = "{version}"',
        "__init__.py",
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
        "--auto-bump",
        action="store_true",
        help="Resolve metadata from the next available MCP patch version",
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
        "--auto-bump",
        action="store_true",
        help="Resolve metadata from the next available MCP patch version",
    )
    validate_parser.add_argument(
        "--expected-tag",
        help="Fail if the computed MCP tag differs from this value",
    )

    write_parser = subparsers.add_parser(
        "write-version",
        help="Write a resolved version into pyproject.toml and __init__.py",
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
    version_group.add_argument(
        "--auto-bump",
        action="store_true",
        help="Write the next available MCP patch version",
    )

    args = parser.parse_args()

    if args.command == "show":
        metadata = load_metadata(
            resolve_version(tag=args.tag, auto_bump=args.auto_bump)
        )
        if args.field:
            print(metadata[args.field])
        else:
            print(json.dumps(metadata, indent=2, ensure_ascii=True))
        return

    if args.command == "validate":
        metadata = load_metadata(
            resolve_version(tag=args.tag, auto_bump=args.auto_bump)
        )
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
            auto_bump=args.auto_bump,
        )
        write_version(version)
        print(version)
        return

    raise SystemExit("Unknown command")


if __name__ == "__main__":
    main()