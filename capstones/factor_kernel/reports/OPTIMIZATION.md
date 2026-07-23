# Factor-kernel profile and optimization decision

## Question and correctness gate

Can an Eigen 3.4 matrix expression replace the scalar C++20 kernel without
changing missing-value or validation semantics, and does it reduce batch time?
Both paths use `validate_factor_inputs`; the Eigen regression compares every
row with the scalar result and rejects NaN weights and zero-column batches.

## Compiler profile

The scalar translation unit was compiled on the report host with:

```text
g++ -std=c++20 -O3 -Icapstones/factor_kernel/include \
  -fopt-info-vec-optimized -fopt-info-vec-missed \
  -c capstones/factor_kernel/src/factor_kernel.cpp
```

GCC 13.2 reported that the row and column loops in `factor_kernel.cpp` were not
vectorized because they contain control flow. That control flow implements the
NaN skip policy. It also reported the result-vector growth path and exception
paths as barriers. This is a compiler optimization report, not a hardware-
counter profile; it identifies the next experiment but does not estimate its
end-to-end benefit.

## Paired measurement

`factor-eigen-comparison.json` contains 31 scalar/Eigen pairs for the same
32,768 x 32 deterministic batch. Even pairs run scalar first and odd pairs run
Eigen first, limiting fixed-order drift. On this host the scalar median was
1,038,400 ns and the Eigen median was 3,973,900 ns; the ratio of medians was
3.83, while the generated median of the 31 within-pair ratios was 3.71.
Checksums matched.
The comparison performs one warm-up and intentionally excludes Python and Arrow
conversion.

## Allocation and copy inventory

- Scalar: one result vector; the caller-owned row-major batch is read in place.
- Current Eigen candidate: one result vector plus materialized value and
  observed-mask matrices before the matrix-vector operations.
- Python binding: validates C-contiguous float64 inputs and returns an owning
  NumPy array; Arrow/NumPy setup stays outside the timed kernel.

## Decision

Do not adopt the current Eigen implementation. Its matrix materialization cost
dominates this narrow kernel. A later experiment may pre-normalize missingness
once, reuse aligned buffers, and inspect generated SIMD instructions and
hardware counters. It must retain the same public oracle before performance is
compared again.
