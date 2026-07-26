from __future__ import annotations

import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]


class CIContractTest(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.workflow = (ROOT / ".github" / "workflows" / "ci.yml").read_text(
            encoding="utf-8"
        )

    def test_book_job_builds_checks_and_uploads_pdf(self) -> None:
        for token in (
            "book:",
            "mkdir -p build/book/chapters build/book/appendices",
            "latexmk",
            "texlive-science",
            "check_pdf_build.py",
            "pdfinfo",
            "upload-artifact",
            "package_release.py --source build/book/main.pdf",
            "build/release/*",
        ):
            self.assertIn(token, self.workflow)

    def test_msvc_matrix_builds_and_runs_ctest(self) -> None:
        for token in (
            "windows-msvc:",
            "windows-latest",
            "build_type: [Debug, Release]",
            "-A x64",
            "VsDevCmd.bat",
            "ctest",
        ):
            self.assertIn(token, self.workflow)

    def test_locked_python_job_builds_real_bindings(self) -> None:
        for token in (
            "locked-python:",
            "uv sync --frozen",
            "pybind11",
            "--no-tests=error",
            "capstone_factor_kernel_pybind_matches_arrow_numpy",
            "ch16_optional_pybind_module_preserves_core_behavior",
        ):
            self.assertIn(token, self.workflow)

    def test_positive_tsan_job_runs_the_spsc_path(self) -> None:
        for token in (
            "thread-sanitizer:",
            "-fsanitize=thread",
            "capstone_spsc_replay_tests",
            "capstone_spsc_replay_preserves_every_message",
        ):
            self.assertIn(token, self.workflow)

    def test_scheduled_optional_dependency_job_builds_eigen_and_catch2(self) -> None:
        for token in (
            "schedule:",
            "optional-dependencies:",
            "FACTOR_KERNEL_BUILD_EIGEN=ON",
            "FACTOR_KERNEL_BUILD_CATCH2=ON",
        ):
            self.assertIn(token, self.workflow)

    def test_tag_release_packages_the_fresh_book_build(self) -> None:
        for token in (
            "refs/tags/v",
            "--source build/book/main.pdf",
            "pdftotext build/book/main.pdf",
            "gh release create",
        ):
            self.assertIn(token, self.workflow)

    def test_first_party_actions_are_pinned_to_commits(self) -> None:
        self.assertNotRegex(
            self.workflow,
            r"uses: actions/(?:checkout|setup-python|upload-artifact|download-artifact)@v\d",
        )

    def test_factor_kernel_static_library_can_link_into_a_python_module(self) -> None:
        cmake = (ROOT / "capstones" / "factor_kernel" / "CMakeLists.txt").read_text(
            encoding="utf-8"
        )
        self.assertIn("POSITION_INDEPENDENT_CODE ON", cmake)

    def test_replay_benchmark_reports_a_portable_compiler_name(self) -> None:
        source = (
            ROOT / "capstones" / "order_book" / "benchmarks" / "replay_benchmark.cpp"
        ).read_text(encoding="utf-8")
        self.assertIn("compiler_name()", source)
        self.assertNotIn("<< __VERSION__", source)


if __name__ == "__main__":
    unittest.main()
