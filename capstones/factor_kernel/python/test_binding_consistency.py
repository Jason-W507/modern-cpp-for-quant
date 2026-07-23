from __future__ import annotations

import sys

import numpy as np
import pyarrow as pa

from binding_runtime import add_windows_dll_directories

add_windows_dll_directories()

import quant_factor_kernel


def reference(values: np.ndarray, weights: np.ndarray) -> np.ndarray:
    observed = ~np.isnan(values)
    numerator = np.nansum(values * weights, axis=1)
    denominator = np.sum(observed * np.abs(weights), axis=1)
    return np.divide(
        numerator,
        denominator,
        out=np.full(values.shape[0], np.nan),
        where=denominator != 0,
    )


def expect_rejected(values: np.ndarray, weights: np.ndarray) -> None:
    try:
        quant_factor_kernel.weighted_factor(values, weights)
    except (TypeError, ValueError):
        return
    raise AssertionError("binding accepted a converted or non-contiguous input")


def main() -> int:
    rows, columns = 4, 3
    flat = pa.array(
        [1.0, 2.0, None, 2.0, None, 4.0, None, None, None, 3.0, 1.0, -1.0],
        type=pa.float64(),
        from_pandas=True,
    )
    arrow_values = pa.FixedSizeListArray.from_arrays(flat, columns)
    values = arrow_values.values.to_numpy(zero_copy_only=False).reshape(rows, columns)
    weights_arrow = pa.array([0.5, -0.25, 0.25], type=pa.float64())
    weights = weights_arrow.to_numpy(zero_copy_only=True)

    actual = quant_factor_kernel.weighted_factor(values, weights)
    expected = reference(values, weights)
    np.testing.assert_allclose(actual, expected, rtol=1e-12, atol=1e-12,
                               equal_nan=True)

    expect_rejected(values.astype(np.float32), weights)
    expect_rejected(np.asfortranarray(values), weights)
    expect_rejected(values, weights.astype(np.float32))
    print("pybind-arrow-consistency-ok rows=4 dtype=float64 contiguous=1")
    return 0


if __name__ == "__main__":
    sys.exit(main())
