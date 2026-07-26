# 事件驱动回测教学项目

这是《Python 量化人的 Modern C++》最终章对应的独立 C++20 项目。它展示一条可审计的单标的事件链，而不是生产交易平台：

```text
CSV -> MarketEvent -> Strategy -> OrderIntent -> Fill -> Portfolio -> Summary
```

`MarketEvent`、`OrderIntent` 和 `Fill` 是经过验证的值对象；显式转换得到的非法 `Side` 也会在入口处被拒绝。组合采用现金账户政策：初始现金必须为有限正数，买入不能超过现金，卖出不能超过持仓。事件时间戳必须按输入顺序非递减；相同时间戳保持输入顺序，调用者负责给出确定的同刻事件顺序。空事件流用缺失的最终组合快照表达，而不是伪造一个代码为空的快照。CSV 错误保留行号、字段与错误代码，CLI 只负责把结构化结果渲染成人类可读文本。

## 复现

在仓库根目录的 Linux 或 WSL 中运行：

```bash
cmake -S project -B build/ch17 -DCMAKE_BUILD_TYPE=Release
cmake --build build/ch17 -j
ctest --test-dir build/ch17 --output-on-failure
./build/ch17/quant_backtest project/data/sample.csv
./build/ch17/quant_backtest project/data/sample.csv 1.0 10.0
```

默认成本与指定成本的输出契约分别为：

```text
backtest-ok events=3 fills=1 cash=7525.000 equity=10100.000 return=1.000000% max-drawdown=0.000000% volatility=0.002880 fee-per-fill=0.000 slippage-bps=0.000
backtest-ok events=3 fills=1 cash=7521.525 equity=10096.525 return=0.965250% max-drawdown=0.034750% volatility=0.003081 fee-per-fill=1.000 slippage-bps=10.000
```

CTest 从公开边界覆盖值对象、CLI 摘要、无效配置、多标的估值边界和带正确性门的基准。数量汇总采用 checked addition；名义金额拒绝非有限乘积与累计结果，并使用补偿求和；波动率通过 Welford 在线算法计算。期望值来自独立手算账本，不由被测实现重新生成。

## 测量

```bash
cmake --build build/ch17 --target ch17_backtest_benchmark -j
ctest --test-dir build/ch17 -R ch17_backtest_benchmark --output-on-failure
```

基准在两轮预热后处理 200,000 个内存事件，输出 11 个微秒样本及其中位数和 IQR。计时前的正确性门要求一笔成交、最终权益 10100、checksum 131300。可信报告还应记录 CPU、编译器、Release 选项和系统负载；这些耗时只是当前环境的观察值，不是可移植承诺。

## 塑造边界的失败实验

组合按代码保存持仓，但最终快照只接收一个估值价格。早期的多标的路径因此可能生成看似合理、实际上无效的总权益。项目现在拒绝混合代码输入，退出码为 2；只有在设计完整的同一时间价格映射和缺失价格政策后，才应开放多标的路径。`failures/mixed_symbols.cpp` 保留该实验，CTest 检查稳定诊断类别。

## 有意保留的限制

- 每次运行只有一个标的和一种现金币种；
- 不借款、不开保证金、不使用杠杆或做空；
- 同步、立即、全量成交；
- 只有固定费用和确定性滑点；
- 不处理交易日历、公司行动、借券、风险服务或真实交易所；
- 不声称示例策略有盈利能力或达到生产标准；
- benchmark 不包含 CSV、磁盘、Python 调用和并发队列。

这些限制本身也是证据的一部分。扩展项目时，应从一个公开行为的最小纵向闭环开始，同时保留已有不变量。

## English summary

This independently buildable C++20 teaching project demonstrates a validated, single-symbol, cash-account backtest pipeline. Its tests cover public behavior, structured CSV failures, time ordering, checked statistics, and a correctness-gated benchmark. It intentionally does not claim production trading semantics or portable latency results.
