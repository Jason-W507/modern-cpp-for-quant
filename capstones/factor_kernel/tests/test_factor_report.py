from __future__ import annotations

import json
import statistics
import sys
from pathlib import Path


def main() -> int:
    report_path, optimization_path, appendix_path = map(Path, sys.argv[1:4])
    report = json.loads(report_path.read_text(encoding="utf-8"))

    paired_median = statistics.median(report["paired_eigen_over_scalar"])
    ratio_of_medians = statistics.median(report["eigen_samples_ns"]) / statistics.median(
        report["scalar_samples_ns"]
    )
    assert abs(report["paired_ratio_median"] - paired_median) < 1e-5
    assert abs(report["eigen_over_scalar"] - ratio_of_medians) < 1e-5

    expected_values = (f"{ratio_of_medians:.2f}", f"{paired_median:.2f}")
    optimization = optimization_path.read_text(encoding="utf-8")
    appendix = appendix_path.read_text(encoding="utf-8")
    assert all(value in optimization for value in expected_values)
    assert all(value in appendix for value in expected_values)
    print("factor-report-contract-ok generated-medians=1 manuscript-match=1")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
