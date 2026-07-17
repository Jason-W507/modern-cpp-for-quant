from __future__ import annotations

import argparse
import re
import subprocess
import sys
from pathlib import Path


def parse_arguments() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Verify an intentional link failure.")
    parser.add_argument("--compiler", type=Path, required=True)
    parser.add_argument("--source", type=Path, required=True)
    parser.add_argument("--work-dir", type=Path, required=True)
    parser.add_argument("--diagnostic", required=True)
    return parser.parse_args()


def is_msvc(compiler: Path) -> bool:
    return compiler.name.lower() in {"cl", "cl.exe", "clang-cl", "clang-cl.exe"}


def compile_command(compiler: Path, source: Path, object_file: Path) -> list[str]:
    if is_msvc(compiler):
        return [
            str(compiler),
            "/nologo",
            "/std:c++20",
            "/c",
            str(source),
            f"/Fo{object_file}",
        ]
    return [str(compiler), "-std=c++20", "-c", str(source), "-o", str(object_file)]


def link_command(compiler: Path, object_file: Path, executable: Path) -> list[str]:
    if is_msvc(compiler):
        return [str(compiler), "/nologo", str(object_file), f"/Fe{executable}"]
    return [str(compiler), str(object_file), "-o", str(executable)]


def run(command: list[str]) -> subprocess.CompletedProcess[str]:
    return subprocess.run(command, text=True, capture_output=True, check=False)


def main() -> int:
    args = parse_arguments()
    args.work_dir.mkdir(parents=True, exist_ok=True)
    object_file = args.work_dir / "missing_definition.obj"
    executable = args.work_dir / (
        "missing_definition.exe" if sys.platform == "win32" else "missing_definition"
    )
    object_file.unlink(missing_ok=True)
    executable.unlink(missing_ok=True)

    compiled = run(compile_command(args.compiler, args.source, object_file))
    if compiled.returncode != 0 or not object_file.is_file():
        print("source did not pass the compilation stage", file=sys.stderr)
        print(compiled.stdout + compiled.stderr, file=sys.stderr)
        return 1

    linked = run(link_command(args.compiler, object_file, executable))
    diagnostics = linked.stdout + linked.stderr
    if linked.returncode == 0:
        executable.unlink(missing_ok=True)
        print("expected linking to fail, but it succeeded", file=sys.stderr)
        return 1
    if re.search(args.diagnostic, diagnostics, re.IGNORECASE) is None:
        print("linking failed without the expected diagnostic category", file=sys.stderr)
        print(diagnostics, file=sys.stderr)
        return 1

    print("link-failure: compilation passed and linking reported a missing definition")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
