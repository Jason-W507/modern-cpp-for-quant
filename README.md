# Python 量化人的 Modern C++

一本面向已会 Python、准备量化投资求职或进入量化开发工作的 C++20 中文自学教程。

**[下载 v0.3.1 PDF（234 页）](https://github.com/Jason-W507/modern-cpp-for-quant/releases/download/v0.3.1/python-quant-modern-cpp-v0.3.1.pdf)** · [校验和](https://github.com/Jason-W507/modern-cpp-for-quant/releases/download/v0.3.1/SHA256SUMS) · [查看最新 Release](https://github.com/Jason-W507/modern-cpp-for-quant/releases/latest)

## 项目概览

全书以 Python 经验为起点，先补齐类型、值语义、对象生命周期、容器和泛型等基础语法，再进入工程化、性能、并发、数值计算与 Python/C++ 互操作。正文由 18 章、五篇和 8 个附录组成；代码包含逐章可构建示例、故意失败的诊断实验、事件驱动回测器，以及两个岗位分流 Capstone。

| 层次 | 主要内容 | 可验证产物 |
|---|---|---|
| C++ 基础 | 类型、引用、类、RAII、STL、模板 | 独立示例、练习与编译诊断 |
| 量化工程 | 数据管线、回测、测试、构建、性能与并发 | CMake/CTest、基准原始样本 |
| 岗位作品集 | 批量因子核、订单簿与行情回放 | Python oracle、协议测试、性能报告 |

## 三条学习路线

| 目标 | 建议顺序 | 重点交付 |
|---|---|---|
| 面试速通 | 第 1--10 章 → 第 12 章 → 附录“面试与练习” | 能解释 Python/C++ 差异，完成基础练习和错误诊断 |
| Quant Developer | 第 1--15 章 → 回测项目 → Order-book Capstone | 事件驱动回测、并发管线、协议解析与有界 replay |
| Research Engineer | 第 1--12、14、16 章 → Factor-kernel Capstone | 数值一致性、批量边界、pybind11 与可复现实验 |

## 两个岗位分流 Capstone

- [`capstones/order_book`](capstones/order_book)：整数 tick、价格—时间优先撮合、32 字节大端序协议、有界乱序缓存、SPSC 交付与 correctness-gated benchmark。它是教学切片，不声称具备交易所级网络、持久化、快照或生产延迟。
- [`capstones/factor_kernel`](capstones/factor_kernel)：独立于 Python 的批量因子核、shape/有限值检查、NumPy/Arrow 独立 oracle 与 pybind11 绑定。绑定在纯 C++ kernel 期间释放 GIL，返回数组拥有自己的内存。

## 30 秒开始构建代码

已有 CMake 3.23+、C++20 编译器和 Python 时，在仓库根目录运行：

```powershell
cmake --preset windows-mingw-release
cmake --build --preset windows-mingw-release
ctest --preset windows-mingw-release --output-on-failure
```

WSL/Linux 使用 `linux-release`。如需真正构建 pybind11 模块，先执行 `uv sync --frozen --python 3.12`，再使用 `windows-mingw-python-release`。根构建提供以下组件开关：

```text
QUANT_BUILD_CHAPTERS       逐章示例与附录
QUANT_BUILD_BACKTEST       事件驱动回测项目
QUANT_BUILD_CAPSTONES      两个岗位分流项目
QUANT_BUILD_NEGATIVE_TESTS 故意失败的诊断实验
QUANT_BUILD_PYTHON         可选 Python 扩展模块
```

## 构建书稿

本机使用 MiKTeX 时可运行：

```powershell
latexmk -xelatex -interaction=nonstopmode -halt-on-error -outdir=build-book main.tex
```

已验证的滚动版 PDF 提交在 [`output/pdf/python-quant-modern-cpp.pdf`](output/pdf/python-quant-modern-cpp.pdf)；正式版本使用 GitHub Release 中的带版本文件名。发布包由 `python tools/package_release.py` 读取根目录 `VERSION` 生成，并同时写出 SHA-256 校验和。

## 仓库结构

```text
parts/                 五篇正文
appendices/            速查、练习、面试与 Capstone 指南
code/ch01..ch16/       逐章可独立构建代码
project/               事件驱动回测器
capstones/             factor kernel 与 order book/replay
tools/                 契约检查、测试与发布工具
docs/authoring/        写作契约与验收记录
output/pdf/            经验证的滚动版 PDF
```

书籍契约采用 18 章、五篇、8 个附录与 235 页规划预算。运行 `python tools/check_book_contract.py` 可检查章节职责、基础语法覆盖、学习单元和代码快照；规划预算与实际 PDF 页数分别核验，避免把目标页数当作构建结果。

## 已验证环境

- GitHub Actions：Ubuntu 24.04，GCC 与 Clang，Debug/Release；另有 Clang ASan/UBSan 作业。
- 本地发布路径：Windows + MinGW + CPython 3.12 锁定环境；WSL/Linux 可使用仓库预设。
- 书稿：XeLaTeX；本项目维护者使用 MiKTeX，发布前会检查引用、严重 overfull 和实际页数。

## 版本、贡献与许可

当前版本由 [`VERSION`](VERSION) 唯一维护。修改行为前请通过公开接口增加回归测试；书稿修改需重编并视觉检查。完整约定见 [`CONTRIBUTING.md`](CONTRIBUTING.md)。

本仓库采用分层许可：原创代码与工程文件使用 MIT License；原创书稿、封面和生成 PDF 使用 CC BY-NC-SA 4.0；`elegantbook.cls` 遵循 LPPL 1.3c。范围与第三方归属见 [`LICENSE.md`](LICENSE.md) 和 [`THIRD_PARTY_NOTICES.md`](THIRD_PARTY_NOTICES.md)。
