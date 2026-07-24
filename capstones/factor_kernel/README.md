# Research Engineer factor-kernel capstone

This capstone keeps the numerical core independent of Python and validates the
same row-major batch through two public seams:

- `weighted_factor(FactorBatchView, weights)` for C++ callers;
- `capstone_factor_kernel_cli BATCH_FILE` for a reproducible NumPy comparison.

The formula skips NaN observations and divides each row's weighted sum by the
sum of absolute weights that were actually observed. A row with no observed
weight returns NaN. Infinite inputs, shape mismatches and row-by-column shape
multiplication overflow are rejected before any span is formed.

```sh
cmake -S capstones/factor_kernel -B build/factor -DBUILD_TESTING=ON
cmake --build build/factor
ctest --test-dir build/factor --output-on-failure
```

When the locked Python environment is active, CTest imports the built
`quant_factor_kernel` module and checks an Arrow fixed-size-list batch against
an independent NumPy formula. Its inputs require float64 C-contiguous arrays
without implicit conversion; the returned array owns its result.
The binding releases the Python GIL only while the validated C++ kernel runs,
then reacquires it before allocating and copying the NumPy result.

```sh
uv sync --python 3.12
cmake --preset windows-mingw-python-release
cmake --build --preset windows-mingw-python-release
ctest --preset windows-mingw-python-release -R capstone_factor_kernel
```

`python/benchmark_factor.py` retains every kernel sample, environment metadata
and checksum in `reports/factor-benchmark.json`. Parquet decode remains outside
the timed kernel boundary. An Eigen/BLAS replacement must preserve the same
public result and checksum before comparing performance.

The optional `capstone_factor_compare` target writes paired scalar/Eigen samples
to `reports/factor-eigen-comparison.json`. `reports/OPTIMIZATION.md` records the
GCC vectorization diagnostics, allocation/copy inventory and the decision not
to adopt the slower materializing Eigen candidate on the report host.
