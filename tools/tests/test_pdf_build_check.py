from __future__ import annotations

import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT))

from tools.check_pdf_build import find_log_failures, parse_page_count


class PdfBuildCheckTest(unittest.TestCase):
    def test_clean_log_has_no_failures(self) -> None:
        self.assertEqual(find_log_failures("Output written on main.pdf"), [])

    def test_undefined_reference_and_overfull_box_are_rejected(self) -> None:
        failures = find_log_failures(
            "LaTeX Warning: There were undefined references.\n"
            "Overfull \\hbox (4.0pt too wide)"
        )
        self.assertEqual(len(failures), 2)

    def test_page_count_is_read_from_pdfinfo(self) -> None:
        self.assertEqual(parse_page_count("Title: book\nPages:          236\n"), 236)

    def test_missing_page_count_is_rejected(self) -> None:
        with self.assertRaisesRegex(ValueError, "Pages"):
            parse_page_count("Title: book\n")


if __name__ == "__main__":
    unittest.main()
