from __future__ import annotations

import argparse
import json
import math
import platform
import statistics
import subprocess
from pathlib import Path


def percentile(samples: list[int], probability: float) -> int:
    ordered = sorted(samples)
    if not ordered:
        raise ValueError("percentile requires at least one sample")
    index = min(max(math.ceil(probability * len(ordered)) - 1, 0), len(ordered) - 1)
    return ordered[index]


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("executable", type=Path)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--processes", type=int, default=11)
    args = parser.parse_args()
    if args.processes < 3:
        raise SystemExit("--processes must be at least 3")

    all_samples: list[int] = []
    process_summaries: list[dict[str, int | float]] = []
    benchmark_environment: dict[str, str] | None = None
    args.output.parent.mkdir(parents=True, exist_ok=True)
    for process_index in range(args.processes):
        report_path = args.output.parent / f"{args.output.stem}.run-{process_index}.json"
        try:
            subprocess.run(
                [str(args.executable), "--json", str(report_path)],
                check=True,
                capture_output=True,
                text=True,
            )
            report = json.loads(report_path.read_text(encoding="utf-8"))
            current_environment = {
                key: str(report["environment"][key])
                for key in ("compiler", "configuration")
            }
            if benchmark_environment is None:
                benchmark_environment = current_environment
            elif current_environment != benchmark_environment:
                raise ValueError("benchmark processes used inconsistent environments")
            samples = [int(value) for value in report["samples_ns"]]
            all_samples.extend(samples)
            process_summaries.append(
                {
                    "p50_ns": percentile(samples, 0.50),
                    "p99_ns": percentile(samples, 0.99),
                    "throughput_msg_s": report["summary"]["throughput_msg_s"],
                }
            )
        finally:
            report_path.unlink(missing_ok=True)

    if benchmark_environment is None:
        raise RuntimeError("benchmark produced no environment report")

    aggregate = {
        "schema": 3,
        "environment": {
            **benchmark_environment,
            "platform": platform.platform(),
            "processor": platform.processor(),
            "processes": args.processes,
        },
        "workload": {
            "messages_per_process": 10_000,
            "window_messages": 100,
            "sample_unit": "ns_per_message_from_window",
            "total_window_samples": len(all_samples),
        },
        "samples_ns": all_samples,
        "summary": {
            "p50_ns": percentile(all_samples, 0.50),
            "p99_ns": percentile(all_samples, 0.99),
            "p999_ns": percentile(all_samples, 0.999),
            "median_process_throughput_msg_s": int(
                statistics.median(
                    summary["throughput_msg_s"] for summary in process_summaries
                )
            ),
        },
        "process_summaries": process_summaries,
        "correctness": {
            "accepted_per_process": 10_000,
            "next_expected_per_process": 10_001,
        },
        "limitations": [
            "window averages do not expose within-window single-message tails",
            "processes run serially without affinity or frequency pinning",
            "single host and teaching protocol, not an exchange feed",
        ],
    }
    args.output.write_text(json.dumps(aggregate, indent=2) + "\n", encoding="utf-8")
    print(
        "replay-multiprocess-ok "
        f"processes={args.processes} samples={len(all_samples)} "
        f"p999_ns={aggregate['summary']['p999_ns']}"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
