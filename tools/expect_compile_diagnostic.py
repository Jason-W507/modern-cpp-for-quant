from __future__ import annotations

import argparse
import re
import subprocess
import sys
from pathlib import Path

from compiler_driver import compile_object_command


def parse_arguments() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Verify a stable compile-time diagnostic category."
    )
    parser.add_argument("--compiler", type=Path, required=True)
    parser.add_argument("--source", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--diagnostic", required=True)
    return parser.parse_args()


def main() -> int:
    args = parse_arguments()
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.unlink(missing_ok=True)
    completed = subprocess.run(
        compile_object_command(
            args.compiler, args.source, args.output, warnings=True
        ),
        text=True,
        capture_output=True,
        check=False,
    )
    diagnostics = completed.stdout + completed.stderr
    if completed.returncode != 0:
        print("source failed instead of compiling with a diagnostic", file=sys.stderr)
        print(diagnostics, file=sys.stderr)
        return 1
    if re.search(args.diagnostic, diagnostics, re.IGNORECASE) is None:
        print("compilation lacked the expected diagnostic category", file=sys.stderr)
        print(diagnostics, file=sys.stderr)
        return 1

    print("compile-diagnostic: source compiled and emitted the expected category")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
