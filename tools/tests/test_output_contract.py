from __future__ import annotations

import subprocess
import sys
import unittest
from pathlib import Path


REPOSITORY_ROOT = Path(__file__).resolve().parents[2]
EXPECT_OUTPUT = REPOSITORY_ROOT / "tools" / "expect_output.py"
EXPECTED = REPOSITORY_ROOT / "tools" / "tests" / "fixtures" / "exact-output.txt"


class ExactOutputCliTest(unittest.TestCase):
    def test_leading_space_is_not_ignored(self) -> None:
        result = subprocess.run(
            [
                sys.executable,
                str(EXPECT_OUTPUT),
                "--expected",
                str(EXPECTED),
                "--",
                sys.executable,
                "-c",
                "import sys; sys.stdout.write(' ready\\n')",
            ],
            text=True,
            capture_output=True,
            check=False,
        )

        self.assertNotEqual(0, result.returncode)
        self.assertIn("expected:", result.stderr)

    def test_unexpected_standard_error_is_rejected(self) -> None:
        result = subprocess.run(
            [
                sys.executable,
                str(EXPECT_OUTPUT),
                "--expected",
                str(EXPECTED),
                "--",
                sys.executable,
                "-c",
                "import sys; sys.stdout.write('ready\\n'); "
                "sys.stderr.write('warning\\n')",
            ],
            text=True,
            capture_output=True,
            check=False,
        )

        self.assertNotEqual(0, result.returncode)
        self.assertIn("unexpected stderr", result.stderr)


if __name__ == "__main__":
    unittest.main()
