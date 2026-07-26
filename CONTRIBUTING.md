# Contributing

感谢你改进这部教程。提交前请先搜索现有 GitHub Issues；较大的内容、接口或目录调整应先开 Issue，说明读者问题、可观察验收和影响范围。

## 问题与修改

- 错别字、链接和局部表述可直接提交小型 PR。
- C++ 行为修改应通过公开边界增加或调整测试，保持章节快照可独立构建。
- 性能结论应附独立判定基准、Release 配置、环境、原始样本和限制。
- 书稿修改后运行 XeLaTeX，并检查未定义引用、严重 overfull 与成品是否能正常读取。
- 不提交 `.venv/`、构建目录、编辑器状态或本机绝对路径。

## 本地验证

```powershell
uv sync --frozen --python 3.12
cmake --preset windows-mingw-python-release
cmake --build --preset windows-mingw-python-release
ctest --preset windows-mingw-python-release --output-on-failure
latexmk -xelatex -interaction=nonstopmode -halt-on-error -outdir=build-review main.tex
```

根据修改范围运行相关子集，并在合并前运行完整测试。提交书稿时请说明视觉检查范围；页数由构建产物动态读取，无需人工同步到文档。

## 许可

提交代码即表示你同意按 MIT License 提供该贡献；提交书稿或原创视觉内容即表示你同意按 CC BY-NC-SA 4.0 提供该贡献。第三方内容必须标明来源并确认许可兼容。
