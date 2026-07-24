from __future__ import annotations

import subprocess
import sys
import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]


class CMakeArchitectureTest(unittest.TestCase):
    def test_root_is_an_orchestrator_with_public_component_switches(self) -> None:
        root_cmake = (ROOT / "CMakeLists.txt").read_text(encoding="utf-8")
        for option in (
            "QUANT_BUILD_CHAPTERS",
            "QUANT_BUILD_BACKTEST",
            "QUANT_BUILD_CAPSTONES",
            "QUANT_BUILD_NEGATIVE_TESTS",
            "QUANT_BUILD_PYTHON",
        ):
            self.assertIn(f"option({option}", root_cmake)
        for chapter in range(1, 17):
            self.assertIn(f"add_subdirectory(code/ch{chapter:02d}", root_cmake)
        self.assertNotIn("code/ch07/src/pipeline.cpp", root_cmake)
        self.assertLess(len(root_cmake.splitlines()), 150)
        integration = (ROOT / "cmake" / "QuantIntegrationTests.cmake").read_text(
            encoding="utf-8"
        )
        self.assertIn("QUANT_BUILD_NEGATIVE_TESTS", integration)

    def test_all_components_can_be_disabled_without_python(self) -> None:
        binary_dir = ROOT / "build" / (
            "cmake-no-components-mingw" if sys.platform == "win32"
            else "cmake-no-components"
        )
        command = [
                "cmake",
                "-S",
                str(ROOT),
                "-B",
                str(binary_dir),
                    "-DBUILD_TESTING=OFF",
                    "-DQUANT_BUILD_CHAPTERS=OFF",
                    "-DQUANT_BUILD_BACKTEST=OFF",
                    "-DQUANT_BUILD_CAPSTONES=OFF",
                    "-DQUANT_BUILD_PYTHON=OFF",
                "-DCMAKE_DISABLE_FIND_PACKAGE_Python3=TRUE",
            ]
        if sys.platform == "win32":
            command.extend(["-G", "MinGW Makefiles", "-DCMAKE_CXX_COMPILER=g++"])
        completed = subprocess.run(
            command,
            capture_output=True,
            text=True,
            encoding="utf-8",
            errors="replace",
            check=False,
        )
        self.assertEqual(
            completed.returncode,
            0,
            msg=completed.stdout + completed.stderr,
        )


if __name__ == "__main__":
    unittest.main()
