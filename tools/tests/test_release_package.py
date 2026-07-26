from __future__ import annotations

import hashlib
import subprocess
import sys
import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]


class ReleasePackageTest(unittest.TestCase):
    def test_package_uses_project_version_and_emits_checksum(self) -> None:
        output_dir = ROOT / "build" / "release-contract"
        completed = subprocess.run(
            [
                sys.executable,
                str(ROOT / "tools" / "package_release.py"),
                "--output-dir",
                str(output_dir),
                "--source",
                str(ROOT / "output" / "pdf" / "python-quant-modern-cpp.pdf"),
            ],
            capture_output=True,
            text=True,
            encoding="utf-8",
            errors="replace",
            check=False,
        )
        self.assertEqual(completed.returncode, 0, completed.stdout + completed.stderr)

        version = (ROOT / "VERSION").read_text(encoding="utf-8").strip()
        artifact = output_dir / f"python-quant-modern-cpp-v{version}.pdf"
        self.assertTrue(artifact.is_file())
        expected_hash = hashlib.sha256(artifact.read_bytes()).hexdigest()
        checksums = (output_dir / "SHA256SUMS").read_text(encoding="utf-8")
        self.assertEqual(checksums, f"{expected_hash}  {artifact.name}\n")


if __name__ == "__main__":
    unittest.main()
