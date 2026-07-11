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

## 模板许可

本书使用 ElegantBook 类文件。本项目中的 `elegantbook.cls` 与模板许可见 `ELEGANTBOOK-LICENSE`，模板项目地址为 <https://github.com/ElegantLaTeX/ElegantBook>。
