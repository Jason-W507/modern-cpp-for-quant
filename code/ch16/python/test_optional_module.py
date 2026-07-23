from __future__ import annotations

import gc
import os
import sys

import numpy as np

if sys.platform == "win32":
    os.add_dll_directory(sys.base_prefix)
    runtime_directory = os.environ.get("QUANT_CH16_DLL_DIR")
    if runtime_directory:
        os.add_dll_directory(runtime_directory)

import quant_ch16


values = np.array([0.25, -0.125, 0.5, 0.375], dtype=np.float64)
assert quant_ch16.sum_returns(values) == 1.0

borrowed = quant_ch16.borrowed_view(values)
del values
gc.collect()
assert borrowed.sum() == 1.0

try:
    quant_ch16.sum_returns(np.array([1.0], dtype=np.float32))
except TypeError as error:
    assert "float64" in str(error)
else:
    raise AssertionError("float32 input must be rejected")

try:
    quant_ch16.sum_returns(np.arange(8.0, dtype=np.float64)[::2])
except ValueError as error:
    assert "contiguous" in str(error)
else:
    raise AssertionError("strided input must be rejected")

print("pybind-ok dtype=float64 contiguous=true owner-kept=true sum=1.000000")
