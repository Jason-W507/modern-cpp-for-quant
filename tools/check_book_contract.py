from __future__ import annotations

import argparse
import json
import re
import sys
from pathlib import Path


CHECKLIST_MARKERS = {
    "prerequisites",
    "learning-output",
    "scenario",
    "syntax-rule",
    "runnable-example",
    "guided-explanation",
    "python-boundary",
    "failure-diagnosis",
    "immediate-practice",
    "code-evidence",
    "feedback",
    "project-connection",
    "publication",
}
POLICY_MARKERS = {
    "complete-listing",
    "intentional-failure",
    "chapter-snapshot",
    "automation",
    "naming",
}
PLACEHOLDER = re.compile(r"(?:TBD|TODO|待补|占位)", re.IGNORECASE)
MARKER = re.compile(r"<!--\s*contract:([a-z-]+)\s*-->")
CHAPTER_REFERENCE = re.compile(r"ch(\d{2})")


def parse_arguments() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Validate the second-edition book contract.")
    parser.add_argument(
        "--root",
        type=Path,
        default=Path(__file__).resolve().parents[1],
        help="Repository root (defaults to the parent of tools/).",
    )
    parser.add_argument(
        "--coverage",
        type=Path,
        help="Override the syntax coverage document (useful for contract tests).",
    )
    return parser.parse_args()


def read_text(path: Path, errors: list[str]) -> str:
    try:
        return path.read_text(encoding="utf-8")
    except OSError as exc:
        errors.append(f"missing or unreadable {path}: {exc}")
        return ""


def load_contract(path: Path, errors: list[str]) -> dict[str, object]:
    text = read_text(path, errors)
    if not text:
        return {}
    try:
        value = json.loads(text)
    except json.JSONDecodeError as exc:
        errors.append(f"invalid JSON in {path}: {exc}")
        return {}
    if not isinstance(value, dict):
        errors.append(f"contract root must be an object: {path}")
        return {}
    return value


def validate_contract(contract: dict[str, object], errors: list[str]) -> tuple[int, int, set[int]]:
    target = contract.get("target_pages")
    parts = contract.get("parts")
    if not isinstance(target, dict):
        errors.append("target_pages must be an object")
        target = {}
    if not isinstance(parts, list):
        errors.append("parts must be a list")
        parts = []

    if len(parts) != 5:
        errors.append(f"expected 5 parts, found {len(parts)}")

    chapters: list[dict[str, object]] = []
    part_pages = 0
    for index, part in enumerate(parts, start=1):
        if not isinstance(part, dict):
            errors.append(f"part {index} must be an object")
            continue
        items = part.get("chapters")
        budget = part.get("page_budget")
        if not isinstance(items, list):
            errors.append(f"part {index} chapters must be a list")
            continue
        if not isinstance(budget, int):
            errors.append(f"part {index} page_budget must be an integer")
            continue
        chapter_budget = sum(
            item.get("page_budget", 0)
            for item in items
            if isinstance(item, dict) and isinstance(item.get("page_budget"), int)
        )
        if chapter_budget != budget:
            errors.append(
                f"part {index} budget is {budget}, but its chapters sum to {chapter_budget}"
            )
        part_pages += budget
        chapters.extend(item for item in items if isinstance(item, dict))

    numbers = [item.get("number") for item in chapters]
    valid_numbers = {value for value in numbers if isinstance(value, int)}
    if len(chapters) != 18:
        errors.append(f"expected 18 chapters, found {len(chapters)}")
    if numbers != list(range(1, 19)):
        errors.append(f"chapter numbers must be contiguous 1..18, found {numbers}")

    for item in chapters:
        number = item.get("number", "?")
        for field in ("title", "learning_output"):
            value = item.get(field)
            if not isinstance(value, str) or not value.strip() or PLACEHOLDER.search(value):
                errors.append(f"chapter {number} has an empty or placeholder {field}")

    minimum = target.get("minimum")
    planned = target.get("planned")
    maximum = target.get("maximum")
    front = target.get("front_matter")
    appendices = target.get("appendices")
    page_values = (minimum, planned, maximum, front, appendices)
    if not all(isinstance(value, int) for value in page_values):
        errors.append("all target page values must be integers")
        return len(chapters), 0, valid_numbers

    if not minimum <= planned <= maximum:
        errors.append(f"planned pages {planned} are outside {minimum}..{maximum}")
    calculated = front + appendices + part_pages
    if calculated != planned:
        errors.append(f"planned pages are {planned}, but budgets sum to {calculated}")

    return len(chapters), planned, valid_numbers


def parse_coverage_rows(text: str) -> list[list[str]]:
    rows: list[list[str]] = []
    for line in text.splitlines():
        if not line.startswith("|"):
            continue
        cells = [cell.strip() for cell in line.strip().strip("|").split("|")]
        if not cells or cells[0] in {"ID", "---"}:
            continue
        rows.append(cells)
    return rows


def validate_coverage(text: str, chapter_numbers: set[int], errors: list[str]) -> int:
    rows = parse_coverage_rows(text)
    if len(rows) < 40:
        errors.append(f"syntax coverage must contain at least 40 concepts, found {len(rows)}")

    identifiers: set[str] = set()
    for row_number, row in enumerate(rows, start=1):
        if len(row) != 5:
            errors.append(f"syntax coverage row {row_number} must have 5 columns, found {len(row)}")
            continue
        identifier, concept, first_taught, practice, reuse = row
        if identifier in identifiers:
            errors.append(f"duplicate syntax coverage ID: {identifier}")
        identifiers.add(identifier)
        if not all((identifier, concept, first_taught, practice, reuse)):
            errors.append(f"syntax coverage row {row_number} contains an empty field")
        for field_name, field_value in (
            ("first teaching", first_taught),
            ("immediate practice", practice),
            ("later reuse", reuse),
        ):
            references = {int(match) for match in CHAPTER_REFERENCE.findall(field_value)}
            if not references:
                errors.append(f"{identifier} has no chapter reference for {field_name}")
            invalid = references - chapter_numbers
            if invalid:
                errors.append(f"{identifier} references invalid chapters {sorted(invalid)}")
        first_references = {int(match) for match in CHAPTER_REFERENCE.findall(first_taught)}
        practice_references = {int(match) for match in CHAPTER_REFERENCE.findall(practice)}
        if (
            first_references
            and practice_references
            and min(practice_references) < min(first_references)
        ):
            errors.append(f"{identifier}: practice precedes first teaching")
    return len(rows)


def validate_markers(
    text: str, expected: set[str], document_name: str, errors: list[str]
) -> None:
    actual = set(MARKER.findall(text))
    missing = expected - actual
    extra = actual - expected
    if missing:
        errors.append(f"{document_name} is missing contract markers: {sorted(missing)}")
    if extra:
        errors.append(f"{document_name} has unknown contract markers: {sorted(extra)}")


def main() -> int:
    args = parse_arguments()
    root = args.root.resolve()
    authoring = root / "docs" / "authoring"
    errors: list[str] = []

    contract = load_contract(authoring / "book-contract.json", errors)
    chapter_count, planned_pages, chapter_numbers = validate_contract(contract, errors)

    coverage_path = args.coverage.resolve() if args.coverage else authoring / "syntax-coverage.md"
    coverage_text = read_text(coverage_path, errors)
    coverage_count = validate_coverage(coverage_text, chapter_numbers, errors)

    checklist_text = read_text(authoring / "chapter-acceptance-checklist.md", errors)
    validate_markers(checklist_text, CHECKLIST_MARKERS, "chapter checklist", errors)

    policy_text = read_text(authoring / "code-example-policy.md", errors)
    validate_markers(policy_text, POLICY_MARKERS, "code policy", errors)

    if errors:
        for error in errors:
            print(f"book-contract: ERROR: {error}", file=sys.stderr)
        return 1

    print(
        "book-contract: valid "
        f"({chapter_count} chapters, {coverage_count} syntax concepts, "
        f"{planned_pages} planned pages)"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
