from __future__ import annotations

import subprocess
import sys
import unittest
from pathlib import Path


REPOSITORY_ROOT = Path(__file__).resolve().parents[2]
CHECKER = REPOSITORY_ROOT / "tools" / "check_book_contract.py"


class BookContractCliTest(unittest.TestCase):
    def test_repository_contract_is_valid(self) -> None:
        result = subprocess.run(
            [sys.executable, str(CHECKER), "--root", str(REPOSITORY_ROOT)],
            capture_output=True,
            text=True,
            check=False,
        )

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
        result = subprocess.run(
            [
                sys.executable,
                str(CHECKER),
                "--root",
                str(REPOSITORY_ROOT),
                "--coverage",
                str(invalid_coverage),
            ],
            capture_output=True,
            text=True,
            check=False,
        )

        self.assertNotEqual(0, result.returncode)
        self.assertIn("practice precedes first teaching", result.stderr)


if __name__ == "__main__":
    unittest.main()
