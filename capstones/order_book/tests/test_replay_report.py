from __future__ import annotations

import json
import sys
from pathlib import Path


benchmark_dir = Path(__file__).resolve().parents[1] / "benchmarks"
sys.path.insert(0, str(benchmark_dir))

from run_replay_benchmark import percentile  # noqa: E402


def main() -> int:
    assert percentile(list(range(100)), 0.50) == 49
    assert percentile(list(range(100)), 0.99) == 98
    assert percentile(list(range(1_100)), 0.999) == 1_098

    report = json.loads(Path(sys.argv[1]).read_text(encoding="utf-8"))
    assert report["environment"]["compiler"]
    assert report["environment"]["configuration"] in {"Debug", "Release"}
    assert "p999_ns" in report["summary"]
    assert all("p999_ns" not in item for item in report["process_summaries"])
    print(
        "replay-report-contract-ok "
        "nearest-rank=1 aggregate-p999=1 environment=1"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
