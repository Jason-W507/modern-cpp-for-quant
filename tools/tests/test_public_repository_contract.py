from __future__ import annotations

import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]


class PublicRepositoryContractTest(unittest.TestCase):
    def test_license_layers_are_explicit(self) -> None:
        overview = (ROOT / "LICENSE.md").read_text(encoding="utf-8")
        self.assertIn("MIT License", overview)
        self.assertIn("CC BY-NC-SA 4.0", overview)
        self.assertIn("LPPL", overview)
        self.assertTrue((ROOT / "LICENSE-CODE").is_file())
        self.assertTrue((ROOT / "LICENSE-CONTENT.md").is_file())
        self.assertTrue((ROOT / "THIRD_PARTY_NOTICES.md").is_file())

    def test_active_tracker_is_github_without_stale_local_tickets(self) -> None:
        issue_tracker = (ROOT / "docs" / "agents" / "issue-tracker.md").read_text(
            encoding="utf-8"
        )
        self.assertIn("Issue tracker: GitHub", issue_tracker)
        self.assertIn("PRs as a request surface: yes", issue_tracker)
        self.assertFalse((ROOT / "tickets.md").exists())
        self.assertFalse((ROOT / ".scratch").exists())
        labels = (ROOT / "docs" / "agents" / "triage-labels.md").read_text(
            encoding="utf-8"
        )
        self.assertNotIn("local Markdown issue tracker", labels)
        self.assertFalse((ROOT / "docs" / "superpowers").exists())
        self.assertFalse(
            (ROOT / "docs" / "archive" / "first-edition-tickets.md").exists()
        )


if __name__ == "__main__":
    unittest.main()
