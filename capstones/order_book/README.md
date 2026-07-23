# Quant Developer order-book and replay capstone

The baseline implements integer-tick price-time matching plus a fixed 32-byte,
big-endian add-order message. `MarketReplay::apply` validates protocol version,
type, declared length, reserved bytes and side before applying the sequence
gate and order book.

```sh
cmake -S capstones/order_book -B build/order-book -DBUILD_TESTING=ON
cmake --build build/order-book
ctest --test-dir build/order-book --output-on-failure
./build/order-book/capstone_replay_benchmark
```

The behavior test freezes malformed, accepted, duplicate and gap outcomes,
buffers out-of-order messages and drains them after the missing sequence
arrives. The SPSC integration test moves 10,000 messages from one producer to
one replay consumer without loss. The benchmark pre-encodes 10,000
messages, gates correctness before reporting p50/p99 nanoseconds and
throughput, and intentionally does not claim production latency. Save raw runs,
CPU/compiler details, affinity and frequency policy before comparing changes.
The committed `reports/replay-benchmark.json` aggregates 1,100 timed windows
from 11 fresh processes and preserves aggregate p50/p99/p99.9, per-process p50/p99 and throughput,
correctness counters, environment and limits. Each sample is nanoseconds per
message for a 100-message window, which avoids the host timer's 0/100 ns
single-call quantization but does not claim within-window single-message tails.
Regenerate it with `benchmarks/run_replay_benchmark.py`.

With Clang, configure `-DCAPSTONE_BUILD_FUZZERS=ON` and run the protocol target
with a bounded corpus. Cancel/replace, snapshots, persistence, real exchange
schemas and clock calibration remain explicit non-goals of this teaching slice.
