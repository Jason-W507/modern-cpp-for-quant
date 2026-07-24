# Research Engineer：因子内核项目

本项目让数值核心保持独立于 Python，并让同一行主序批次通过两个公开边界接受验证：

- `weighted_factor(FactorBatchView, weights)` 面向 C++ 调用者；
- `capstone_factor_kernel_cli BATCH_FILE` 提供可复现的 NumPy 对照。

公式跳过 NaN 观测，并用每行实际观测到的绝对权重之和归一化加权和；某行没有任何有效权重时返回 NaN。形成任何 `span` 之前，接口会拒绝无穷输入、形状不匹配以及行列乘法溢出。

```sh
cmake -S capstones/factor_kernel -B build/factor -DBUILD_TESTING=ON
cmake --build build/factor
ctest --test-dir build/factor --output-on-failure
```

锁定的 Python 环境启用后，CTest 会导入已构建的 `quant_factor_kernel` 模块，并把 Arrow 定长列表批次与独立 NumPy 公式比较。输入要求 float64、C-contiguous 数组，且不允许隐式类型转换；返回数组独占自己的结果内存。绑定只在已验证的 C++ 内核运行期间释放 Python GIL，在分配并复制 NumPy 结果前重新取得它。

```sh
uv sync --frozen --python 3.12
cmake --preset windows-mingw-python-release
cmake --build --preset windows-mingw-python-release
ctest --preset windows-mingw-python-release -R capstone_factor_kernel
```

`python/benchmark_factor.py` 把每个内核样本、环境元数据和校验和保存到 `reports/factor-benchmark.json`；Parquet 解码不进入计时边界。采用 Eigen/BLAS 替代实现之前，必须先保持同一公开结果和校验和。

可选目标 `capstone_factor_compare` 把标量/Eigen 配对样本写入 `reports/factor-eigen-comparison.json`。`reports/OPTIMIZATION.md` 记录 GCC 向量化诊断、分配/复制清单，以及在报告宿主上不采用较慢的物化 Eigen 候选实现这一决定。
