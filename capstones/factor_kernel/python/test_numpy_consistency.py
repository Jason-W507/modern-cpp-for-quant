from __future__ import annotations

import math
import subprocess
import sys
from pathlib import Path

import numpy as np


def reference(values: np.ndarray, weights: np.ndarray) -> np.ndarray:
    observed = ~np.isnan(values)
    numerators = np.nansum(values * weights, axis=1)
    denominators = np.sum(observed * np.abs(weights), axis=1)
    return np.divide(
        numerators,
        denominators,
        out=np.full(values.shape[0], np.nan),
        where=denominators != 0,
    )


def main() -> int:
    executable = Path(sys.argv[1])
    work_directory = Path(sys.argv[2])
    work_directory.mkdir(parents=True, exist_ok=True)
    values = np.array(
        [[1.0, 2.0, np.nan], [2.0, np.nan, 4.0],
         [np.nan, np.nan, np.nan], [3.0, 1.0, -1.0]],
        dtype=np.float64,
    )
    weights = np.array([0.5, -0.25, 0.25], dtype=np.float64)
    expected = reference(values, weights)

    data = work_directory / "batch.txt"
    tokens = ["4 3", " ".join(map(str, weights))]
    tokens.extend(" ".join(map(str, row)) for row in values)
    data.write_text("\n".join(tokens) + "\n", encoding="utf-8")
    completed = subprocess.run(
        [str(executable), str(data)],
        check=False,
        capture_output=True,
        text=True,
    )

    if completed.returncode != 0:
        print(completed.stderr, file=sys.stderr)
        return 2
    actual = np.array(
        [math.nan if token == "nan" else float(token)
         for token in completed.stdout.strip().split(",")]
    )
    if not np.allclose(actual, expected, rtol=1e-12, atol=1e-12,
                       equal_nan=True):
        print(f"expected={expected} actual={actual}", file=sys.stderr)
        return 3
    print("numpy-consistency-ok rows=4 dtype=float64 contiguous=1")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
