# 《Python 量化人的 Modern C++》第二版扩写

Status: ready-for-agent

## Problem Statement

当前教程已经具备完整目录、可构建代码、回测项目和 62 页 PDF，但正文更接近结构完整的初稿：多数小节只有一段概述，没有系统教授 C++ 基础语法，也缺少逐步解释、失败实验和足够的练习反馈。目标读者虽然熟练使用 Python，却可能完全没有 C++ 经验，因此无法仅靠当前正文独立跨越程序结构、静态类型、控制流、函数、容器、类、对象生命周期和构建诊断等基础门槛。

## Solution

将教程重构为约 145–155 页、18 章、五篇的第二版。新增五章连续的零基础教学，并对现有章节拆分、去重和扩写，使每个核心小节形成“问题场景、语法规则、可运行例子、逐步解释、Python 差异、失败诊断、立即练习”的七步教学闭环。基础篇以一个可运行的行情分析命令行程序收口，后续将它逐步演化为事件驱动回测引擎。完整代码清单必须对应真实、可编译的源码；关键项目阶段保留可独立构建的章节快照。全书采用 Linux/WSL、GCC、CMake、GDB 和 sanitizer 作为唯一正文主线，并保留必要的 Windows 差异提示。

## User Stories

1. As a Python 量化读者, I want the book to assume no C++ knowledge, so that I can start without consulting a second beginner textbook.
2. As a first-time C++ learner, I want to understand source files, `main`, headers, namespaces, statements and compiler commands, so that I can build and run my first program independently.
3. As a Python developer, I want C++ variables, objects and initialization contrasted with Python names and objects, so that I do not transfer the wrong mental model.
4. As a learner, I want built-in types, literals, operators and narrowing explained with runnable examples, so that I can predict both values and representations.
5. As a learner, I want `if`, `switch`, `for` and `while` taught through market-data scenarios, so that I can write basic control flow without copying unexplained code.
6. As a learner, I want functions, scope, declarations, definitions and return values taught before advanced abstractions, so that I can organize non-trivial programs.
7. As a Python developer, I want pass-by-value, references, pointers and `const` compared with Python argument passing, so that I can reason about mutation and lifetime.
8. As a learner, I want headers, implementation files and namespaces introduced through a working multi-file program, so that compiler and linker errors become understandable.
9. As a learner, I want `string`, `vector`, arrays, range loops, iterators, algorithms and lambdas introduced in a deliberate sequence, so that I can process tabular market data safely.
10. As a learner, I want CSV input taught as an end-to-end task with malformed-input cases, so that file I/O and validation are grounded in realistic work.
11. As a learner, I want `struct`, `class`, scoped enums, constructors and member functions taught through a small market-data model, so that object-oriented syntax is not assumed.
12. As a learner, I want the foundation section to end with a multi-file market-data analyzer, so that I can demonstrate a concrete capability before entering advanced C++.
13. As a Modern C++ learner, I want object lifetime, value categories, copy/move operations and RAII explained from first principles, so that ownership rules are not memorized as slogans.
14. As a developer reading production or interview code, I want raw pointers, C arrays, array decay and manual allocation covered as low-level literacy, so that I can diagnose unsafe code while preferring modern defaults.
15. As a learner, I want smart pointers taught only after direct values and RAII, so that I choose ownership tools from semantics rather than habit.
16. As a quant developer, I want STL algorithms and ranges connected to data pipelines, so that I can replace manual loops when the abstraction improves clarity.
17. As a component author, I want classes, interfaces, invariants and value semantics applied to orders, fills and portfolios, so that I can design reliable domain boundaries.
18. As a C++ learner, I want templates, concepts and polymorphism built from previously mastered function and class syntax, so that compiler diagnostics remain interpretable.
19. As a production developer, I want error categories, exception safety and observability connected to concrete failure paths, so that I can distinguish invalid input, normal absence and programmer error.
20. As a learner, I want target-based CMake taught separately from testing tools, so that build relationships are not compressed into a command list.
21. As a learner, I want debugging, tests, sanitizer and static analysis demonstrated on intentional failures, so that I can diagnose faults instead of only reading warnings about them.
22. As a performance-oriented developer, I want data layout, allocation, benchmarking and profiling taught as evidence-producing experiments, so that I avoid cargo-cult optimization.
23. As a Python quant developer, I want concurrency, numerical stability and pybind11 boundaries connected to Python experience, so that I know when C++ changes the cost and correctness model.
24. As a project learner, I want the foundation analyzer to evolve into the existing backtest engine, so that I can observe refactoring from script-like code to tested components.
25. As an offline reader, I want key project stages to remain independently buildable, so that later refactoring does not invalidate the code shown in earlier chapters.
26. As a self-learner, I want each chapter to state prerequisites, an observable output task and self-check questions, so that I can tell whether I am ready to continue.
27. As a stuck learner, I want all foundation exercises and selected advanced exercises to have runnable solutions, so that I can recover without abandoning the book.
28. As an interview candidate, I want interview checkpoints distributed through the technical chapters, so that preparation follows understanding rather than a detached question bank.
29. As a job candidate, I want a substantial final chapter on code questions, system design, project evidence and communication, so that the technical work can become a credible portfolio story.
30. As a new quant C++ hire, I want a practical first-three-month route, so that the book remains useful after the interview.
31. As a maintainer, I want every complete listing compiled from real source, so that book text and code cannot silently diverge.
32. As a maintainer, I want chapter-level learning acceptance in addition to book-level build checks, so that a green PDF does not conceal a pedagogically incomplete chapter.
33. As a reader using Windows, I want concise MSVC and MinGW differences beside the WSL/Linux mainline, so that platform variation is visible without duplicating every instruction.
34. As a reader, I want the second edition to stay within roughly 145–155 pages, so that added depth does not turn into an unfocused language encyclopedia.

## Implementation Decisions

- The target reader is proficient in Python and may have zero C++ knowledge.
- The second edition has 18 chapters in five parts and targets approximately 145–155 rendered pages.
- The five foundation chapters cover: first build and diagnostics; types, expressions and control flow; functions, references, `const` and multi-file organization; standard data structures, algorithms and CSV; structs, classes and the completed market-data analyzer.
- The next five chapters cover: object lifetime, value categories, RAII and ownership; STL and ranges; class and interface design; templates and concepts; error handling and observability.
- Two reliable-engineering chapters separate CMake and dependency modeling from testing, debugging and quality tools.
- Four performance chapters cover data layout and allocation; benchmarking and profiling; concurrency and backpressure; numerical stability and Python interoperability.
- The final two chapters assemble the event-driven backtest engine and connect the work to interviews, portfolio evidence and onboarding.
- Existing material is reorganized and deduplicated; the old chapters are not merely shifted behind new introductory chapters.
- Each core section follows the seven-step teaching closure: problem scenario, syntax or rule, runnable example, guided explanation, Python boundary, failure diagnosis and immediate exercise. Project application follows where useful.
- Complete listings come from real C++20 source and participate in the build. Intentionally broken examples are clearly isolated and document their expected diagnostic.
- The main learning environment is Linux/WSL with GCC, CMake, GDB and sanitizer. Windows alternatives are compact notes rather than a parallel tutorial.
- Modern standard-library and RAII defaults are taught before raw-pointer and manual-memory literacy.
- The market-data analyzer and backtest engine form one evolutionary project line.
- Key project milestones are stored as independently buildable chapter snapshots, accepting limited duplication to preserve reproducibility, as recorded in ADR 0001.
- Foundation exercises receive runnable, guided solutions. Advanced chapters provide full solutions for core exercises and structured hints plus acceptance checks for the rest.
- Technical instruction remains the dominant content. Interview checkpoints appear in each chapter, with approximately 12–15 pages reserved for the final career chapter.
- Implementation begins with two calibration chapters: the foundation chapter on types/control flow and the advanced chapter on object lifetime/RAII. Their accepted teaching scale becomes the model for later chapters.
- Page targets are guardrails, not acceptance by word count. Depth is judged by the chapter learning unit and reader task.

## Testing Decisions

- The primary test seam is the chapter learning unit. A chapter passes when a reader with the declared prerequisites can build its complete examples, follow its failure experiment, produce its stated output task, answer its self-check and locate appropriate feedback.
- Tests verify externally visible behavior and documented outputs rather than internal helper calls or private representation.
- Every complete example is configured as a C++20 build target or is compiled through an equivalent automated check.
- Intentionally invalid examples are excluded from the normal green build and checked against a documented diagnostic category rather than compiler-specific full text.
- Project-stage snapshots must configure, build and run independently from a clean checkout.
- The existing project behavior seams remain in use: CSV input, strategy decision, execution and portfolio behavior, and end-to-end backtest results.
- Foundation exercise solutions must run and demonstrate the stated task; advanced solution checks focus on the exercises designated as core.
- The full C++ test suite continues to run in a configuration where assertions cannot silently disappear.
- The LaTeX build must produce the latest PDF with resolved references, readable code listings, no missing glyphs and no fatal or material overflow errors.
- Page count is checked after the two calibration chapters and after each part, allowing content redistribution while keeping the final target range.
- A syntax coverage matrix maps each required beginner concept to its first teaching location, practice location and later reuse, preventing assumed-but-untaught syntax.
- A chapter acceptance checklist checks prerequisites, seven-step core sections, Python boundaries, failure labs, immediate exercises, project connection and answer coverage.

## Out of Scope

- Teaching Python fundamentals, NumPy fundamentals or financial theory.
- Becoming a complete reference for every C++20 language or standard-library feature.
- Detailed template metaprogramming, coroutines, modules, SIMD intrinsics or lock-free algorithm design.
- Production exchange connectivity, full order-book simulation, live trading, alpha research or profitable strategy design.
- Maintaining equal, line-by-line tutorials for GCC, Clang, MSVC and MinGW.
- Providing a long-form complete solution for every advanced exercise.
- Replacing the existing ElegantBook visual design or modifying the original template source directory.
- Treating an arbitrary page count increase as proof of teaching quality.

## Further Notes

- The current 62-page edition is the source inventory, not disposable work: its verified examples, backtest components, testing seams, benchmark and job-oriented framing should be reused where they meet the new teaching standard.
- The current source has no subsection-level hierarchy in the 14 main chapters and most chapters contain very few complete listings; the rewrite must introduce genuine instructional layers rather than elongating existing paragraphs.
- The planned 150-page allocation is: 6 pages front matter, 38 pages foundation, 38 pages language and components, 13 pages reliable engineering, 25 pages performance and interoperability, 18 pages project and career, and 12 pages appendices. Chapter budgets remain guardrails rather than word-count targets.
- The specification is ready for an agent once the ticket breakdown is approved.
