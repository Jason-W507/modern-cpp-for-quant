from __future__ import annotations

import argparse
import os
import platform
import re
import shutil
import subprocess
import sys
from pathlib import Path

from compiler_driver import is_msvc


SKIP = 77


def parse_arguments() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Compile and verify an intentional sanitizer failure."
    )
    parser.add_argument("--compiler", type=Path, required=True)
    parser.add_argument("--source", type=Path, required=True)
    parser.add_argument("--work-dir", type=Path, required=True)
    parser.add_argument(
        "--sanitizer", choices=("address", "undefined", "thread"), required=True
    )
    parser.add_argument("--diagnostic", required=True)
    return parser.parse_args()


def main() -> int:
    args = parse_arguments()
    if os.name == "nt" or is_msvc(args.compiler):
        print("sanitizer-failure: SKIP: this lab runs on Linux GCC or Clang")
        return SKIP

    args.work_dir.mkdir(parents=True, exist_ok=True)
    executable = args.work_dir / "failure"
    compile_command = [
        str(args.compiler),
        "-std=c++20",
        "-O0",
        "-g",
        f"-fsanitize={args.sanitizer}",
        "-fno-omit-frame-pointer",
        str(args.source),
        "-o",
        str(executable),
    ]
    compiled = subprocess.run(
        compile_command, text=True, capture_output=True, check=False
    )
    if compiled.returncode != 0:
        print(compiled.stdout + compiled.stderr, file=sys.stderr)
        return 1

    environment = os.environ.copy()
    if args.sanitizer == "address":
        environment["ASAN_OPTIONS"] = "detect_leaks=0:halt_on_error=1"
    elif args.sanitizer == "undefined":
        environment["UBSAN_OPTIONS"] = "halt_on_error=1:print_stacktrace=1"
    else:
        environment["TSAN_OPTIONS"] = "halt_on_error=1:exitcode=66"
    completed = subprocess.run(
        [str(executable)],
        text=True,
        capture_output=True,
        check=False,
        env=environment,
    )
    diagnostics = completed.stdout + completed.stderr
    if args.sanitizer == "thread" and "unexpected memory mapping" in diagnostics:
        setarch = shutil.which("setarch")
        if setarch is None:
            print("sanitizer-failure: SKIP: TSan needs setarch on this host")
            return SKIP
        completed = subprocess.run(
            [setarch, platform.machine(), "-R", str(executable)],
            text=True,
            capture_output=True,
            check=False,
            env=environment,
        )
        diagnostics = completed.stdout + completed.stderr
        if "Operation not permitted" in diagnostics:
            print("sanitizer-failure: SKIP: host forbids disabling process ASLR")
            return SKIP
    if completed.returncode == 0:
        print("expected sanitizer execution to fail", file=sys.stderr)
        return 1
    if re.search(args.diagnostic, diagnostics, re.IGNORECASE) is None:
        print("sanitizer output lacked the expected category", file=sys.stderr)
        print(diagnostics, file=sys.stderr)
        return 1
    print(f"sanitizer-failure: observed {args.sanitizer} diagnostic")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
