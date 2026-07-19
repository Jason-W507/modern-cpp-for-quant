from __future__ import annotations

import _quant_ch16_gil_probe


held_progress = _quant_ch16_gil_probe.hold_gil_probe()
released_progress = _quant_ch16_gil_probe.release_gil_probe()

assert held_progress is False
assert released_progress is True
print(
    "gil-diagnostic "
    f"held-progress={str(held_progress).lower()} "
    f"released-progress={str(released_progress).lower()}"
)
