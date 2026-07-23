from __future__ import annotations

import os
import sys


def add_windows_dll_directories() -> None:
    if sys.platform != "win32":
        return
    os.add_dll_directory(sys.base_prefix)
    runtime_directory = os.environ.get("FACTOR_KERNEL_DLL_DIR")
    if runtime_directory:
        os.add_dll_directory(runtime_directory)
