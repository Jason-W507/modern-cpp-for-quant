from __future__ import annotations

import argparse
import re
import subprocess
import sys
from pathlib import Path

from compiler_driver import compile_object_command, link_executable_command


def parse_arguments() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Verify an intentional link failure.")
    parser.add_argument("--compiler", type=Path, required=True)
    parser.add_argument("--source", type=Path, required=True)
    parser.add_argument("--work-dir", type=Path, required=True)
    parser.add_argument("--diagnostic", required=True)
    return parser.parse_args()


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

    compiled = run(compile_object_command(args.compiler, args.source, object_file))
    if compiled.returncode != 0 or not object_file.is_file():
        print("source did not pass the compilation stage", file=sys.stderr)
        print(compiled.stdout + compiled.stderr, file=sys.stderr)
        return 1

    linked = run(link_executable_command(args.compiler, object_file, executable))
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
