# C++ 零基础语法覆盖矩阵

本矩阵防止正文在使用语法后才补充解释。`首次教学` 必须给出规则和最小例子，`立即练习` 必须要求读者亲手产生可观察结果，`后续复用` 则验证知识没有只出现一次。

章节引用使用稳定编号 `ch01`–`ch18`；章节标题可以调整，但编号职责以第二版书籍契约为准。

| ID | 概念 | 首次教学 | 立即练习 | 后续复用 |
| --- | --- | --- | --- | --- |
| program-entry | `main`、返回码与程序入口 | ch01 | ch01 | ch03, ch05 |
| include-directive | `#include` 与声明可见性 | ch01 | ch01 | ch03, ch11 |
| namespace | 命名空间与限定名 | ch01 | ch01 | ch03, ch08 |
| statement-block | 语句、分号与花括号块 | ch01 | ch01 | ch02, ch03 |
| comments | 行注释与块注释 | ch01 | ch01 | ch02, ch12 |
| compile-link-run | 预处理、编译、链接与运行 | ch01 | ch01 | ch03, ch11 |
| diagnostics | 编译器诊断的首个根因 | ch01 | ch01 | ch03, ch12 |
| variable-declaration | 变量声明、对象与名称 | ch02 | ch02 | ch05, ch06 |
| initialization | 花括号初始化与默认初始化 | ch02 | ch02 | ch05, ch06 |
| built-in-types | 整数、浮点、字符与布尔类型 | ch02 | ch02 | ch05, ch16 |
| fixed-width-integers | 固定宽度整数与范围 | ch02 | ch02 | ch05, ch13 |
| literals | 整数、浮点、字符和字符串字面量 | ch02 | ch02 | ch04, ch16 |
| auto | `auto` 类型推导的边界 | ch02 | ch02 | ch07, ch09 |
| casts | 显式转换与 `static_cast` | ch02 | ch02 | ch13, ch16 |
| arithmetic-operators | 算术、余数与复合赋值 | ch02 | ch02 | ch14, ch16 |
| comparison-boolean | 比较、逻辑运算与短路 | ch02 | ch02 | ch05, ch10 |
| if-else | 条件分支 | ch02 | ch02 | ch05, ch10 |
| switch | 枚举分派与 `switch` | ch02 | ch02 | ch05, ch10 |
| for-loop | 计数循环 | ch02 | ch02 | ch04, ch14 |
| while-loop | 条件循环与终止条件 | ch02 | ch02 | ch04, ch15 |
| scope | 块作用域与名称隐藏 | ch02 | ch02 | ch03, ch06 |
| function-declaration | 函数声明、定义与调用 | ch03 | ch03 | ch09, ch11 |
| parameters-return | 参数、返回值与早返回 | ch03 | ch03 | ch08, ch10 |
| overloads | 函数重载与候选选择 | ch03 | ch03 | ch09, ch16 |
| header-source | 头文件、实现文件与翻译单元 | ch03 | ch03 | ch11, ch17 |
| include-guard | include guard 与重复包含 | ch03 | ch03 | ch11 |
| pass-by-value | 按值参数与独立对象 | ch03 | ch03 | ch06, ch08 |
| references | 左值引用与借用 | ch03 | ch03 | ch06, ch08 |
| const | 常量对象、只读引用与成员 `const` | ch03 | ch03 | ch06, ch08 |
| pointers | 地址、空指针与非拥有观察 | ch03 | ch03 | ch06, ch15 |
| string | `std::string` 的拥有语义 | ch04 | ch04 | ch05, ch10 |
| vector | `std::vector`、大小与容量 | ch04 | ch04 | ch07, ch13 |
| array | `std::array` 与 C 数组识读 | ch04 | ch04 | ch06, ch13 |
| range-for | 范围 `for` 与元素绑定 | ch04 | ch04 | ch07, ch14 |
| iterators | 迭代器、端点与失效 | ch04 | ch04 | ch07, ch13 |
| algorithms | 查找、排序、转换与归约 | ch04 | ch04 | ch07, ch16 |
| lambda | 捕获、参数和返回 | ch04 | ch04 | ch07, ch15 |
| file-io | 文件流、打开状态与逐行读取 | ch04 | ch04 | ch10, ch17 |
| csv-validation | CSV 字段、转换与错误上下文 | ch04 | ch04 | ch10, ch17 |
| enum-class | 作用域枚举与穷举分支 | ch05 | ch05 | ch08, ch10 |
| struct | 聚合数据记录 | ch05 | ch05 | ch08, ch13 |
| class-access | `class`、访问控制与公开接口 | ch05 | ch05 | ch08, ch09 |
| constructors | 构造函数与构造后有效 | ch05 | ch05 | ch06, ch08 |
| member-functions | 成员访问、成员函数与 `this` | ch05 | ch05 | ch08, ch09 |
| exception-basics | 抛出、捕获与栈展开概览 | ch05 | ch05 | ch06, ch10 |

维护规则：新增基础语法必须增加一行；若某概念第一次出现的章节提前，必须同时移动首次教学与立即练习，不能只改后续引用。
