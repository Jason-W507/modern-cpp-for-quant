# Python 量化人的 Modern C++

本项目是一部面向已会 Python 的量化开发者和研究工程师的 C++20 中文教程，使用 ElegantBook 排版，并配有可构建示例和一个事件驱动回测引擎。

## 构建书稿

在安装 MiKTeX 后，从项目根目录运行：

```powershell
latexmk -xelatex -interaction=nonstopmode -halt-on-error -outdir=build main.tex
```

生成文件为 `build/main.pdf`。

## 构建代码

```powershell
cmake -S . -B build-cpp -DCMAKE_BUILD_TYPE=Release
cmake --build build-cpp
ctest --test-dir build-cpp --output-on-failure
```

主线采用 C++20 和 Linux GCC/Clang；Windows 上可使用 GCC、Clang 或 MSVC。示例不依赖大型量化框架。

性能基准 `layout_benchmark` 使用固定的 100 万条合成记录，预热后交替测量 AoS/SoA 各 20 次，输出中位数、IQR 与 checksum 一致性。结果只适用于记录过的硬件、编译器和负载；正式比较应保存命令与原始样本，并跨独立进程重复。

## 验证第二版书籍契约

第二版采用 18 章、五篇、计划 150 页的机器可读契约，并维护零基础语法覆盖矩阵、章节学习单元清单以及代码/项目快照约定。直接运行：

```powershell
python tools/check_book_contract.py
```

该检查也已接入 CTest；缺少章节职责、预算不闭合、语法练习早于首次教学或验收文档结构损坏都会让测试失败。作者约定见 `docs/authoring/`。

## 模板许可

本书使用 ElegantBook 类文件。本项目中的 `elegantbook.cls` 与模板许可见 `ELEGANTBOOK-LICENSE`，模板项目地址为 <https://github.com/ElegantLaTeX/ElegantBook>。
