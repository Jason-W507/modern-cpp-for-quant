from __future__ import annotations

import argparse
import subprocess
import sys
from pathlib import Path

from output_contract import matches_exactly, read_expected


def parse_arguments() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Verify a command's exit status, standard output, and error."
    )
    parser.add_argument("--expected-exit", type=int, required=True)
    parser.add_argument("--expected-stdout", type=Path)
    parser.add_argument("--expected-stderr", type=Path)
    parser.add_argument("command", nargs=argparse.REMAINDER)
    return parser.parse_args()


def expected_text(path: Path | None) -> str:
    return "" if path is None else read_expected(path)


def main() -> int:
    args = parse_arguments()
    command = args.command[1:] if args.command[:1] == ["--"] else args.command
    if not command:
        print("expect-process: no command provided", file=sys.stderr)
        return 2

    completed = subprocess.run(command, text=True, capture_output=True, check=False)
    expected_stdout = expected_text(args.expected_stdout)
    expected_stderr = expected_text(args.expected_stderr)

    if (
        completed.returncode != args.expected_exit
        or not matches_exactly(completed.stdout, expected_stdout)
        or not matches_exactly(completed.stderr, expected_stderr)
    ):
        print(
            f"expected exit {args.expected_exit}, stdout {expected_stdout!r}, "
            f"stderr {expected_stderr!r}",
            file=sys.stderr,
        )
        print(
            f"actual exit {completed.returncode}, stdout {completed.stdout!r}, "
            f"stderr {completed.stderr!r}",
            file=sys.stderr,
        )
        return 1

    print("process-contract: exit, stdout, and stderr matched")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
