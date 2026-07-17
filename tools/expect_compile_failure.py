from __future__ import annotations

import argparse
import re
import subprocess
import sys
from pathlib import Path


def parse_arguments() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Verify an intentional compile failure.")
    parser.add_argument("--compiler", type=Path, required=True)
    parser.add_argument("--source", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--diagnostic", required=True)
    return parser.parse_args()


def compile_command(compiler: Path, source: Path, output: Path) -> list[str]:
    executable = compiler.name.lower()
    if executable in {"cl", "cl.exe", "clang-cl", "clang-cl.exe"}:
        return [
            str(compiler),
            "/nologo",
            "/std:c++20",
            "/c",
            str(source),
            f"/Fo{output}",
        ]
    return [str(compiler), "-std=c++20", "-c", str(source), "-o", str(output)]


def main() -> int:
    args = parse_arguments()
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.unlink(missing_ok=True)
    completed = subprocess.run(
        compile_command(args.compiler, args.source, args.output),
        text=True,
        capture_output=True,
        check=False,
    )

    diagnostics = completed.stdout + completed.stderr
    if completed.returncode == 0:
        args.output.unlink(missing_ok=True)
        print("expected compilation to fail, but it succeeded", file=sys.stderr)
        return 1
    if re.search(args.diagnostic, diagnostics, re.IGNORECASE) is None:
        print("compilation failed without the expected diagnostic category", file=sys.stderr)
        print(diagnostics, file=sys.stderr)
        return 1

    print("compile-failure: observed expected diagnostic category")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
