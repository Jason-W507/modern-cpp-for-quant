from __future__ import annotations

import argparse
import json
import platform
import statistics
import time
from pathlib import Path

import numpy as np
import pyarrow as pa

from binding_runtime import add_windows_dll_directories

add_windows_dll_directories()

import quant_factor_kernel


def percentile(samples: list[int], probability: float) -> int:
    ordered = sorted(samples)
    return ordered[min(int(probability * len(ordered)), len(ordered) - 1)]


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--rows", type=int, default=65536)
    parser.add_argument("--columns", type=int, default=32)
    parser.add_argument("--repeats", type=int, default=31)
    args = parser.parse_args()

    generator = np.random.default_rng(20260723)
    owned = generator.normal(size=(args.rows, args.columns)).astype(np.float64)
    owned[::97, 3] = np.nan
    arrow = pa.FixedSizeListArray.from_arrays(
        pa.array(owned.reshape(-1), type=pa.float64()), args.columns)
    values = arrow.values.to_numpy(zero_copy_only=True).reshape(
        args.rows, args.columns)
    weights = pa.array(np.linspace(-1.0, 1.0, args.columns)).to_numpy(
        zero_copy_only=True)

    quant_factor_kernel.weighted_factor(values, weights)
    samples: list[int] = []
    checksum = 0.0
    for _ in range(args.repeats):
        start = time.perf_counter_ns()
        result = quant_factor_kernel.weighted_factor(values, weights)
        samples.append(time.perf_counter_ns() - start)
        checksum = float(np.nansum(result))

    report = {
        "schema": 1,
        "environment": {
            "python": platform.python_version(),
            "platform": platform.platform(),
            "processor": platform.processor(),
            "numpy": np.__version__,
            "pyarrow": pa.__version__,
        },
        "workload": {
            "rows": args.rows,
            "columns": args.columns,
            "repeats": args.repeats,
            "dtype": str(values.dtype),
            "c_contiguous": bool(values.flags.c_contiguous),
            "arrow_child_zero_copy": True,
        },
        "samples_ns": samples,
        "summary_ns": {
            "median": int(statistics.median(samples)),
            "p99": percentile(samples, 0.99),
            "min": min(samples),
            "max": max(samples),
        },
        "checksum": checksum,
        "limitations": [
            "single host and process",
            "wall-clock samples are not hardware counters",
            "Parquet decode time is outside the measured kernel boundary",
        ],
    }
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(f"factor-benchmark-ok samples={args.repeats} rows={args.rows}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
