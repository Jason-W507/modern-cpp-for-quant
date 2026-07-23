# Research Engineer factor-kernel capstone

This capstone keeps the numerical core independent of Python and validates the
same row-major batch through two public seams:

- `weighted_factor(FactorBatchView, weights)` for C++ callers;
- `capstone_factor_kernel_cli BATCH_FILE` for a reproducible NumPy comparison.

The formula skips NaN observations and divides each row's weighted sum by the
sum of absolute weights that were actually observed. A row with no observed
weight returns NaN. Infinite inputs and shape mismatches are rejected.

```sh
cmake -S capstones/factor_kernel -B build/factor -DBUILD_TESTING=ON
cmake --build build/factor
ctest --test-dir build/factor --output-on-failure
```

When NumPy is installed, CTest runs an independent Python reference. When a
CMake package for pybind11 is available, the optional `quant_factor_kernel`
module is also built. Its input arguments require float64, C-contiguous arrays
without implicit conversion; the returned array owns a copy of the result.

Arrow/Parquet decoding and Eigen/BLAS kernels are deliberate next seams, not
claims of the current dependency-free baseline. A portfolio report should
measure decode, boundary copy, kernel, and allocation time separately.
