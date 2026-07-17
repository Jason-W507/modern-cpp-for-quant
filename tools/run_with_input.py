from __future__ import annotations

import argparse
import subprocess
import sys
from pathlib import Path


def parse_arguments() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Run a program with fixed standard input.")
    parser.add_argument("--input", type=Path, required=True)
    parser.add_argument("--expected", required=True)
    parser.add_argument("command", nargs=argparse.REMAINDER)
    return parser.parse_args()


def main() -> int:
    args = parse_arguments()
    command = args.command[1:] if args.command[:1] == ["--"] else args.command
    if not command:
        print("run-with-input: no command provided", file=sys.stderr)
        return 2

    completed = subprocess.run(
        command,
        input=args.input.read_text(encoding="utf-8"),
        text=True,
        capture_output=True,
        check=False,
    )
    if completed.returncode != 0:
        print(completed.stderr, end="", file=sys.stderr)
        return completed.returncode

    actual = completed.stdout.strip()
    if actual != args.expected:
        print(f"expected: {args.expected!r}", file=sys.stderr)
        print(f"actual:   {actual!r}", file=sys.stderr)
        return 1

    print(actual)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
