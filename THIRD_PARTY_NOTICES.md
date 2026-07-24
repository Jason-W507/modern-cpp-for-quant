# Third-party notices

## ElegantBook

本项目使用 ElegantBook 文档类。`elegantbook.cls` 来源于 ElegantLaTeX/ElegantBook，并保留其 LaTeX Project Public License 1.3c 许可；许可证全文见 `ELEGANTBOOK-LICENSE`，上游项目为 <https://github.com/ElegantLaTeX/ElegantBook>。

## TeX、C++ 与 Python 依赖

MiKTeX/TeX 宏包、CMake、编译器、pybind11、NumPy、PyArrow、pytest、Eigen 和 Catch2 均为外部工具或依赖，不因出现在构建说明、锁文件或测试中而被本仓库重新许可。其使用受各自上游许可证约束。

## 样例数据与 benchmark 报告

`project/data/` 中的数据为教学用合成样例，不包含真实客户、交易所或个人数据；原创样例数据及本机生成的 benchmark JSON 随代码按 MIT License 提供。性能数字只描述报告中记录的环境和 workload。
