from __future__ import annotations

import argparse
import re
import subprocess
import sys


OUTPUT = re.compile(
    r"benchmark-ok events=200000 samples=11 warmups=2 "
    r"equity=10100 fills=1 raw-us=(\d+(?:,\d+){10}) "
    r"median-us=(\d+) iqr-us=(\d+) checksum=([0-9.eE+-]+)"
)


def parse_arguments() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Validate Chapter 17 benchmark output and its statistics."
    )
    parser.add_argument("command", nargs=argparse.REMAINDER)
    arguments = parser.parse_args()
    if arguments.command[:1] == ["--"]:
        arguments.command = arguments.command[1:]
    if not arguments.command:
        parser.error("a benchmark command is required after --")
    return arguments


def main() -> int:
    arguments = parse_arguments()
    completed = subprocess.run(
        arguments.command,
        capture_output=True,
        text=True,
        check=False,
    )
    if completed.returncode != 0:
        sys.stderr.write(completed.stdout)
        sys.stderr.write(completed.stderr)
        return completed.returncode

    match = OUTPUT.fullmatch(completed.stdout.strip())
    if match is None:
        sys.stderr.write(f"invalid benchmark output: {completed.stdout!r}\n")
        return 2

    samples = [int(value) for value in match.group(1).split(",")]
    ordered = sorted(samples)
    reported_median = int(match.group(2))
    reported_iqr = int(match.group(3))
    reported_checksum = float(match.group(4))
    expected_median = ordered[len(ordered) // 2]
    expected_iqr = ordered[(len(ordered) * 3) // 4] - ordered[len(ordered) // 4]
    if reported_median != expected_median or reported_iqr != expected_iqr:
        sys.stderr.write(
            "benchmark statistics do not match the 11 raw microsecond samples\n"
        )
        return 2
    if reported_checksum != 131_300.0:
        sys.stderr.write("benchmark checksum does not match the hand ledger\n")
        return 2

    sys.stdout.write(completed.stdout)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
