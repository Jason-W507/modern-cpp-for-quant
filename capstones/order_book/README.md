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

The behavior test freezes malformed, accepted, duplicate and gap outcomes and
checks the final book by public queries. The benchmark pre-encodes 10,000
messages, gates correctness before reporting p50/p99/p99.9 nanoseconds and
throughput, and intentionally does not claim production latency. Save raw runs,
CPU/compiler details, affinity and frequency policy before comparing changes.

Cancel/replace, snapshots, retransmission, persistence, real exchange schemas,
clock calibration and an SPSC handoff remain explicit non-goals of this slice.
