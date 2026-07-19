from __future__ import annotations

import subprocess
import sys
import unittest
from pathlib import Path


REPOSITORY_ROOT = Path(__file__).resolve().parents[2]
CHECKER = REPOSITORY_ROOT / "tools" / "check_book_contract.py"


class BookContractCliTest(unittest.TestCase):
    def run_checker(self, *arguments: str) -> subprocess.CompletedProcess[str]:
        return subprocess.run(
            [
                sys.executable,
                str(CHECKER),
                "--root",
                str(REPOSITORY_ROOT),
                *arguments,
            ],
            capture_output=True,
            text=True,
            check=False,
        )

    def test_repository_contract_is_valid(self) -> None:
        result = self.run_checker()

        self.assertEqual(0, result.returncode, result.stdout + result.stderr)
        self.assertIn("18 chapters", result.stdout)
        self.assertIn("150 planned pages", result.stdout)

    def test_practice_cannot_precede_first_teaching(self) -> None:
        invalid_coverage = (
            REPOSITORY_ROOT
            / "tools"
            / "tests"
            / "fixtures"
            / "practice-before-teaching.md"
        )
        result = self.run_checker(
            "--coverage",
            str(invalid_coverage),
        )

        self.assertNotEqual(0, result.returncode)
        self.assertIn("practice precedes first teaching", result.stderr)

    def test_accepted_chapter_requires_learning_unit_evidence(self) -> None:
        invalid_units = (
            REPOSITORY_ROOT
            / "tools"
            / "tests"
            / "fixtures"
            / "accepted-without-evidence.json"
        )
        result = self.run_checker("--units", str(invalid_units))

        self.assertNotEqual(0, result.returncode)
        self.assertIn("accepted chapter 1 is missing evidence", result.stderr)

    def test_only_declared_calibration_chapters_can_use_contract_prerequisites(self) -> None:
        invalid_units = (
            REPOSITORY_ROOT
            / "tools"
            / "tests"
            / "fixtures"
            / "undeclared-calibration-contract.json"
        )
        result = self.run_checker("--units", str(invalid_units))

        self.assertNotEqual(0, result.returncode)
        self.assertIn("chapter 3 is not a declared calibration chapter", result.stderr)

    def test_every_tex_listing_requires_code_evidence(self) -> None:
        invalid_evidence = (
            REPOSITORY_ROOT
            / "tools"
            / "tests"
            / "fixtures"
            / "missing-listing-evidence.json"
        )
        result = self.run_checker("--code-evidence", str(invalid_evidence))

        self.assertNotEqual(0, result.returncode)
        self.assertIn("unregistered complete listings", result.stderr)

    def test_main_must_include_every_contract_chapter_in_order(self) -> None:
        invalid_main = (
            REPOSITORY_ROOT
            / "tools"
            / "tests"
            / "fixtures"
            / "main-missing-final-chapter.tex"
        )
        result = self.run_checker("--main", str(invalid_main))

        self.assertNotEqual(0, result.returncode)
        self.assertIn("main chapter includes must match contract order", result.stderr)

    def test_commented_chapter_include_does_not_count_as_published(self) -> None:
        invalid_main = (
            REPOSITORY_ROOT
            / "tools"
            / "tests"
            / "fixtures"
            / "main-commented-final-chapter.tex"
        )
        result = self.run_checker("--main", str(invalid_main))

        self.assertNotEqual(0, result.returncode)
        self.assertIn("main chapter includes must match contract order", result.stderr)

    def test_accepted_appendices_require_complete_learning_recovery_evidence(self) -> None:
        invalid_appendices = (
            REPOSITORY_ROOT
            / "tools"
            / "tests"
            / "fixtures"
            / "incomplete-appendix-units.json"
        )
        result = self.run_checker("--appendices", str(invalid_appendices))

        self.assertNotEqual(0, result.returncode)
        self.assertIn("appendix registry must cover", result.stderr)

    def test_published_appendix_registry_must_be_accepted(self) -> None:
        draft_path = (
            REPOSITORY_ROOT
            / "tools"
            / "tests"
            / "fixtures"
            / "draft-appendix-units.json"
        )
        result = self.run_checker("--appendices", str(draft_path))

        self.assertNotEqual(0, result.returncode)
        self.assertIn("appendix registry state must be accepted", result.stderr)


if __name__ == "__main__":
    unittest.main()
