# Quant Developer：订单簿与行情重放项目

基线实现整数价格档位上的价格—时间优先撮合，以及固定 32 字节、大端序的新增订单消息。`MarketReplay::apply` 在消息进入序号门和订单簿之前，验证帧尺寸、协议版本、消息类型、声明长度、保留字节与买卖方向，并用 `DecodeError` 保留具体失败类别。订单入口与档位查询也会拒绝显式构造的非法 `Side` 值。

```sh
cmake -S capstones/order_book -B build/order-book -DBUILD_TESTING=ON
cmake --build build/order-book
ctest --test-dir build/order-book --output-on-failure
./build/order-book/capstone_replay_benchmark
```

行为测试固定了畸形消息、序号已接受、订单簿已接受、订单簿拒绝、重复消息与序号缺口等结果。乱序缓冲区同时受最大序号距离和消息数量约束；容量耗尽时，基线策略拒绝最新消息。恢复消息若被订单簿拒绝，结果会明确呈现这一事实，不会计作成功入簿。SPSC 集成测试让一个生产者向一个重放消费者传递 10,000 条消息，并验证没有丢失。

基准预编码 10,000 条消息，先通过正确性门，再报告 p50/p99 纳秒数与吞吐量；它不声称代表生产延迟。比较改动前后，应保存原始运行数据、CPU/编译器信息、亲和性与频率策略。已提交的 `reports/replay-benchmark.json` 汇总 11 个新进程中的 1,100 个计时窗口，保留总体 p50/p99/p99.9、逐进程 p50/p99 和吞吐量、正确性计数、环境及限制。每个样本表示 100 条消息窗口中的单条平均纳秒数；这种口径绕开宿主计时器对单次调用的 0/100 ns 量化，却不能解释为窗口内部的单条消息尾延迟。可用 `benchmarks/run_replay_benchmark.py` 重新生成报告。

使用 Clang 时，可配置 `-DCAPSTONE_BUILD_FUZZERS=ON`，并用有界语料运行协议模糊测试目标。取消/替换、快照、持久化、真实交易所协议和时钟校准仍是这个教学切片的明确非目标。
