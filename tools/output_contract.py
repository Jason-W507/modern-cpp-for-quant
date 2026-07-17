from __future__ import annotations

from pathlib import Path


def read_expected(path: Path) -> str:
    """Read expected text while preserving all content except platform newlines."""
    return path.read_text(encoding="utf-8")


def matches_exactly(actual: str, expected: str) -> bool:
    return actual == expected
