from __future__ import annotations

import argparse
import os
import re
import subprocess
import sys
import tempfile
from pathlib import Path


SKIP = 77


def parse_arguments() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Verify an AddressSanitizer failure.")
    parser.add_argument("--source", type=Path, required=True)
    parser.add_argument("--diagnostic", required=True)
    return parser.parse_args()


def run(command: list[str]) -> subprocess.CompletedProcess[str]:
    return subprocess.run(command, text=True, capture_output=True, check=False)


def verify_failure(completed: subprocess.CompletedProcess[str], pattern: str) -> int:
    diagnostics = completed.stdout + completed.stderr
    if completed.returncode == 0:
        print("expected sanitizer execution to fail, but it succeeded", file=sys.stderr)
        return 1
    if re.search(pattern, diagnostics, re.IGNORECASE) is None:
        print("sanitizer failed without the expected diagnostic category", file=sys.stderr)
        print(diagnostics, file=sys.stderr)
        return 1
    print("asan-failure: observed expected diagnostic category")
    return 0


def run_in_wsl(source: Path, pattern: str) -> int:
    try:
        available = run(["wsl.exe", "--", "bash", "-lc", "true"])
    except FileNotFoundError:
        print("asan-failure: SKIP: WSL is unavailable")
        return SKIP
    if available.returncode != 0:
        print("asan-failure: SKIP: WSL cannot be started in this environment")
        return SKIP

    resolved = source.resolve()
    drive = resolved.drive.rstrip(":").lower()
    relative_parts = resolved.parts[1:]
    source_path = f"/mnt/{drive}/" + "/".join(relative_parts)

    temporary = run(["wsl.exe", "--", "mktemp", "/tmp/cpp-asan-XXXXXX"])
    if temporary.returncode != 0:
        print("asan-failure: SKIP: WSL temporary directory is unavailable")
        return SKIP

    binary = temporary.stdout.strip()
    try:
        compiled = run(
            [
                "wsl.exe",
                "--",
                "g++",
                "-std=c++20",
                "-O0",
                "-g",
                "-fsanitize=address",
                "-fno-omit-frame-pointer",
                source_path,
                "-o",
                binary,
            ]
        )
        if compiled.returncode != 0:
            print(compiled.stdout + compiled.stderr, file=sys.stderr)
            return 1
        completed = run(
            [
                "wsl.exe",
                "--",
                "env",
                "ASAN_OPTIONS=detect_leaks=0:halt_on_error=1",
                binary,
            ]
        )
        return verify_failure(completed, pattern)
    finally:
        run(["wsl.exe", "--", "rm", "-f", binary])


def run_natively(source: Path, pattern: str) -> int:
    with tempfile.TemporaryDirectory(prefix="cpp-asan-") as directory:
        binary = Path(directory) / "failure"
        compiled = run(
            [
                "c++",
                "-std=c++20",
                "-O0",
                "-g",
                "-fsanitize=address",
                "-fno-omit-frame-pointer",
                str(source),
                "-o",
                str(binary),
            ]
        )
        if compiled.returncode != 0:
            print(compiled.stdout + compiled.stderr, file=sys.stderr)
            return 1
        completed = run([str(binary)])
    return verify_failure(completed, pattern)


def main() -> int:
    args = parse_arguments()
    if os.name == "nt":
        return run_in_wsl(args.source, args.diagnostic)
    return run_natively(args.source, args.diagnostic)


if __name__ == "__main__":
    raise SystemExit(main())
