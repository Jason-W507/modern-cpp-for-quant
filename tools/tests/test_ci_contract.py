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
            "latexmk",
            "check_pdf_build.py",
            "pdfinfo",
            "upload-artifact",
            "python-quant-modern-cpp.pdf",
        ):
            self.assertIn(token, self.workflow)

    def test_msvc_matrix_builds_and_runs_ctest(self) -> None:
        for token in (
            "windows-msvc:",
            "windows-latest",
            "build_type: [Debug, Release]",
            "-A x64",
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


if __name__ == "__main__":
    unittest.main()
