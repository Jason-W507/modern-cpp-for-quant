from __future__ import annotations

import argparse
import hashlib
import shutil
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]


def package_release(output_dir: Path, source: Path) -> Path:
    version = (ROOT / "VERSION").read_text(encoding="utf-8").strip()
    if not source.is_file():
        raise FileNotFoundError(f"freshly built PDF not found: {source}")

    output_dir.mkdir(parents=True, exist_ok=True)
    artifact = output_dir / f"python-quant-modern-cpp-v{version}.pdf"
    shutil.copy2(source, artifact)
    digest = hashlib.sha256(artifact.read_bytes()).hexdigest()
    (output_dir / "SHA256SUMS").write_text(
        f"{digest}  {artifact.name}\n", encoding="utf-8", newline="\n"
    )
    return artifact


def main() -> int:
    parser = argparse.ArgumentParser(description="Package the versioned book PDF")
    parser.add_argument(
        "--output-dir", type=Path, default=ROOT / "dist", help="release directory"
    )
    parser.add_argument(
        "--source",
        type=Path,
        required=True,
        help="PDF produced by the current build",
    )
    args = parser.parse_args()
    artifact = package_release(args.output_dir.resolve(), args.source.resolve())
    print(f"release-package-ok {artifact}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
