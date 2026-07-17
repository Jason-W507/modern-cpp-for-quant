from __future__ import annotations

import argparse
import re
import subprocess
import sys
from pathlib import Path


def parse_arguments() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Verify an intentional runtime failure.")
    parser.add_argument("--compiler", type=Path, required=True)
    parser.add_argument("--source", type=Path, required=True)
    parser.add_argument("--work-dir", type=Path, required=True)
    parser.add_argument("--expected-exit", type=int, required=True)
    parser.add_argument("--diagnostic", required=True)
    return parser.parse_args()


def compile_command(compiler: Path, source: Path, executable: Path) -> list[str]:
    if compiler.name.lower() in {"cl", "cl.exe", "clang-cl", "clang-cl.exe"}:
        return [
            str(compiler),
            "/nologo",
            "/std:c++20",
            str(source),
            f"/Fe{executable}",
        ]
    return [str(compiler), "-std=c++20", str(source), "-o", str(executable)]


def main() -> int:
    args = parse_arguments()
    args.work_dir.mkdir(parents=True, exist_ok=True)
    executable = args.work_dir / (
        "startup_failure.exe" if sys.platform == "win32" else "startup_failure"
    )
    executable.unlink(missing_ok=True)

    compiled = subprocess.run(
        compile_command(args.compiler, args.source, executable),
        text=True,
        capture_output=True,
        check=False,
    )
    if compiled.returncode != 0:
        print("runtime experiment did not compile", file=sys.stderr)
        print(compiled.stdout + compiled.stderr, file=sys.stderr)
        return 1

    completed = subprocess.run(
        [str(executable)], text=True, capture_output=True, check=False
    )
    diagnostics = completed.stdout + completed.stderr
    if completed.returncode != args.expected_exit:
        print(
            f"expected exit {args.expected_exit}, observed {completed.returncode}",
            file=sys.stderr,
        )
        print(diagnostics, file=sys.stderr)
        return 1
    if re.search(args.diagnostic, diagnostics, re.IGNORECASE) is None:
        print("runtime failure lacked the expected diagnostic", file=sys.stderr)
        print(diagnostics, file=sys.stderr)
        return 1

    print("runtime-failure: observed expected diagnostic and exit status")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
