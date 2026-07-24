from __future__ import annotations

import importlib.util
import re
import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]


class VersionContractTest(unittest.TestCase):
    def test_one_version_file_drives_build_book_and_python(self) -> None:
        version = (ROOT / "VERSION").read_text(encoding="utf-8").strip()
        self.assertRegex(version, r"^\d+\.\d+\.\d+$")

        cmake = (ROOT / "CMakeLists.txt").read_text(encoding="utf-8")
        self.assertIn("file(STRINGS ${CMAKE_CURRENT_SOURCE_DIR}/VERSION", cmake)
        self.assertIn("VERSION ${QUANT_PROJECT_VERSION}", cmake)

        main_tex = (ROOT / "main.tex").read_text(encoding="utf-8")
        self.assertIn("\\openin\\versionfile=VERSION", main_tex)
        self.assertIn("\\version{\\BookVersion}", main_tex)
        self.assertNotRegex(main_tex, r"\\version\{\d")

        pyproject = (ROOT / "pyproject.toml").read_text(encoding="utf-8")
        self.assertNotRegex(pyproject, r'(?m)^version\s*=')

        module_path = ROOT / "tools" / "project_version.py"
        spec = importlib.util.spec_from_file_location("project_version", module_path)
        self.assertIsNotNone(spec)
        self.assertIsNotNone(spec.loader)
        module = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(module)
        self.assertEqual(version, module.read_version(ROOT))


if __name__ == "__main__":
    unittest.main()
