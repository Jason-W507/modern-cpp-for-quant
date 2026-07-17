from __future__ import annotations

import argparse
import subprocess
import sys
from pathlib import Path

from output_contract import matches_exactly, read_expected


def parse_arguments() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Configure, build, and test a CMake project in a clean directory."
    )
    parser.add_argument("--cmake", type=Path, required=True)
    parser.add_argument("--source", type=Path, required=True)
    parser.add_argument("--expected", type=Path, required=True)
    parser.add_argument("--generator", required=True)
    parser.add_argument("--compiler", type=Path, required=True)
    parser.add_argument("--work-root", type=Path, required=True)
    parser.add_argument("--config", default="")
    return parser.parse_args()


def run(command: list[str]) -> subprocess.CompletedProcess[str]:
    completed = subprocess.run(command, text=True, capture_output=True, check=False)
    if completed.returncode != 0:
        print(completed.stdout, end="", file=sys.stderr)
        print(completed.stderr, end="", file=sys.stderr)
    return completed


def main() -> int:
    args = parse_arguments()
    build = args.work_root / "ch01-clean-build"
    if run([str(args.cmake), "-E", "remove_directory", str(build)]).returncode != 0:
        return 1

    configure = run(
        [
            str(args.cmake),
            "-S",
            str(args.source),
            "-B",
            str(build),
            "-G",
            args.generator,
            f"-DCMAKE_CXX_COMPILER={args.compiler}",
        ]
    )
    if configure.returncode != 0:
        return 1

    build_command = [
        str(args.cmake),
        "--build",
        str(build),
        "--target",
        "ch01_first_program",
    ]
    if args.config:
        build_command.extend(["--config", args.config])
    if run(build_command).returncode != 0:
        return 1

    executable_name = (
        "ch01_first_program.exe" if sys.platform == "win32" else "ch01_first_program"
    )
    candidates = [path for path in build.rglob(executable_name) if path.is_file()]
    if len(candidates) != 1:
        print(f"expected one built executable, found {candidates}", file=sys.stderr)
        return 1

    completed = run([str(candidates[0])])
    expected = read_expected(args.expected)
    if completed.returncode != 0 or not matches_exactly(completed.stdout, expected):
        print(
            f"expected exit 0 and output {expected!r}, observed "
            f"exit {completed.returncode} and {completed.stdout!r}",
            file=sys.stderr,
        )
        return 1

    print("clean-cmake-project: configure, build, and test passed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
