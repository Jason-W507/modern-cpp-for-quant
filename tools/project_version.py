from __future__ import annotations

import re
from pathlib import Path


VERSION_PATTERN = re.compile(r"^\d+\.\d+\.\d+$")


def read_version(root: Path | None = None) -> str:
    repository_root = root or Path(__file__).resolve().parents[1]
    version = (repository_root / "VERSION").read_text(encoding="utf-8").strip()
    if not VERSION_PATTERN.fullmatch(version):
        raise ValueError(f"invalid VERSION value: {version!r}")
    return version


if __name__ == "__main__":
    print(read_version())
