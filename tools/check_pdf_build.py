from __future__ import annotations

import argparse
import re
import subprocess
from pathlib import Path


LOG_FAILURES = (
    ("undefined references", re.compile(r"There were undefined references", re.I)),
    ("undefined citation", re.compile(r"Citation .+ undefined", re.I)),
    ("unfinished bibliography", re.compile(r"Please \(re\)run Biber", re.I)),
    ("overfull box", re.compile(r"Overfull \\[hv]box", re.I)),
    ("fatal TeX error", re.compile(r"Fatal error occurred|Emergency stop", re.I)),
)


def find_log_failures(log_text: str) -> list[str]:
    return [label for label, pattern in LOG_FAILURES if pattern.search(log_text)]


def parse_page_count(pdfinfo_text: str) -> int:
    match = re.search(r"^Pages:\s*(\d+)\s*$", pdfinfo_text, re.MULTILINE)
    if match is None:
        raise ValueError("pdfinfo output does not contain a Pages field")
    return int(match.group(1))


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Reject incomplete TeX logs and implausible PDF page counts."
    )
    parser.add_argument("--log", type=Path, required=True)
    parser.add_argument("--pdf", type=Path, required=True)
    parser.add_argument("--pdfinfo", default="pdfinfo")
    parser.add_argument("--min-pages", type=int, default=200)
    parser.add_argument("--max-pages", type=int, default=280)
    args = parser.parse_args()

    if not args.log.is_file():
        raise SystemExit(f"missing TeX log: {args.log}")
    if not args.pdf.is_file():
        raise SystemExit(f"missing PDF: {args.pdf}")

    failures = find_log_failures(args.log.read_text(encoding="utf-8", errors="replace"))
    if failures:
        raise SystemExit("PDF build log failed checks: " + ", ".join(failures))

    result = subprocess.run(
        [args.pdfinfo, str(args.pdf)],
        check=True,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
    )
    pages = parse_page_count(result.stdout)
    if not args.min_pages <= pages <= args.max_pages:
        raise SystemExit(
            f"page count {pages} is outside [{args.min_pages}, {args.max_pages}]"
        )
    print(f"pdf-build-ok pages={pages}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
