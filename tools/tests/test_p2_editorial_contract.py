from __future__ import annotations

import re
import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]


class P2EditorialContractTest(unittest.TestCase):
    def test_every_chapter_has_one_consolidated_opening(self) -> None:
        chapters = sorted((ROOT / "chapters").glob("ch[0-9][0-9].tex"))
        self.assertEqual(len(chapters), 18)
        for chapter in chapters:
            text = chapter.read_text(encoding="utf-8")
            self.assertEqual(text.count(r"\chapteropening{"), 1, chapter.name)
            self.assertNotIn(r"\begin{introduction}", text, chapter.name)
            self.assertNotIn(r"\chaptermeta", text, chapter.name)
            self.assertNotRegex(text, r"\\section\{本章(?:输出|任务)", chapter.name)

    def test_later_chapters_do_not_restore_the_rigid_scene_template(self) -> None:
        for number in range(11, 18):
            text = (ROOT / "chapters" / f"ch{number:02d}.tex").read_text(
                encoding="utf-8"
            )
            self.assertNotIn(r"\subsection{场景：", text, f"ch{number:02d}")

    def test_preface_routes_and_test_semantics_are_explicit(self) -> None:
        preface = (ROOT / "chapters" / "preface.tex").read_text(encoding="utf-8")
        for route in ("C++ 零基础", "Research Engineer", "Quant Developer"):
            self.assertIn(route, preface)
        self.assertIn("逐字符比较只用于", preface)
        for evidence in ("不变量", "独立手算", "容差", "状态转换", "计数守恒"):
            self.assertIn(evidence, preface)

    def test_bibliography_is_driven_by_direct_citations(self) -> None:
        main = (ROOT / "main.tex").read_text(encoding="utf-8")
        self.assertNotIn(r"\nocite{*}", main)
        manuscript = "\n".join(
            path.read_text(encoding="utf-8")
            for directory in (ROOT / "chapters", ROOT / "appendices")
            for path in directory.glob("*.tex")
        )
        cited = set(re.findall(r"\\(?:auto|text)?cite\{([^}]+)\}", manuscript))
        flattened = {key.strip() for group in cited for key in group.split(",")}
        self.assertTrue(
            {
                "cppdraft",
                "williams2019concurrency",
                "clangasan",
                "cmakepresets",
                "higham2002accuracy",
                "pybind11docs",
                "coreguidelines",
                "googlebenchmark",
                "gregg2020systems",
            }.issubset(flattened)
        )

    def test_cxx23_map_names_ranges_to(self) -> None:
        appendix = (ROOT / "appendices" / "modern_cpp_map.tex").read_text(
            encoding="utf-8"
        )
        self.assertIn(r"\texttt{std::ranges::to}", appendix)

    def test_authoring_checklist_allows_theme_specific_later_chapters(self) -> None:
        checklist = (
            ROOT / "docs" / "authoring" / "chapter-acceptance-checklist.md"
        ).read_text(encoding="utf-8")
        self.assertIn("第 1--10 章", checklist)
        self.assertIn("问题—设计—证据—限制", checklist)
        self.assertIn("假设—实验协议—原始数据—结论—威胁", checklist)
        self.assertNotIn("每个核心教学节都必须", checklist)
        self.assertNotIn("每个核心小节都应能找到以下七项", checklist)

    def test_chinese_prose_uses_chinese_owner_and_oracle_terms(self) -> None:
        prose_files = [ROOT / "README.md"]
        prose_files.extend(
            path
            for path in (ROOT / "chapters").glob("*.tex")
            if path.name != "preface.tex"
        )
        prose_files.extend(
            path
            for path in (ROOT / "appendices").glob("*.tex")
            if path.name != "glossary.tex"
        )
        for path in prose_files:
            text = path.read_text(encoding="utf-8")
            prose = "\n".join(
                line
                for line in text.splitlines()
                if "oracle=" not in line
                and "oracle:" not in line
                and "unique_owner" not in line
                and "owner-missing" not in line
                and not line.lstrip().startswith(r"\item[\texttt{owner}")
                and not line.lstrip().startswith(r"\item[\texttt{oracle}")
            )
            self.assertNotRegex(prose, r"(?<![A-Za-z_])oracle(?![A-Za-z_])", path.name)
            self.assertNotRegex(prose, r"(?<![A-Za-z_])owner(?![A-Za-z_])", path.name)

    def test_chinese_terms_are_not_glued_to_english_or_spaced_from_chinese(self) -> None:
        terms = r"(?:公开边界|策略边界|独立判定基准|手算基准|业务判定基准|最小纵向闭环)"
        bad_patterns = (
            rf"[A-Za-z]{terms}",
            rf"{terms}[A-Za-z]",
            rf"{terms} (?=[\u4e00-\u9fff])",
            rf"[\u4e00-\u9fff] (?={terms})",
        )
        for directory in (ROOT / "chapters", ROOT / "appendices"):
            for path in directory.glob("*.tex"):
                text = path.read_text(encoding="utf-8")
                for pattern in bad_patterns:
                    self.assertNotRegex(text, pattern, f"{path.name}: {pattern}")

    def test_negative_compile_examples_accept_msvc_diagnostic_wording(self) -> None:
        ch08 = (ROOT / "code" / "ch08" / "CMakeLists.txt").read_text(
            encoding="utf-8"
        )
        ch09 = (ROOT / "code" / "ch09" / "CMakeLists.txt").read_text(
            encoding="utf-8"
        )

        self.assertIn("did not override", ch08)
        self.assertIn("left of.*on_event.*must have class/struct/union", ch09)


if __name__ == "__main__":
    unittest.main()
