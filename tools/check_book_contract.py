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
LISTING_REFERENCE = re.compile(r"\\lstinputlisting(?:\[[^\]]*\])?\{([^}]+)\}")


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
    parser.add_argument(
        "--units",
        type=Path,
        help="Override the chapter learning-unit registry (useful for contract tests).",
    )
    parser.add_argument(
        "--code-evidence",
        type=Path,
        help="Override the code-evidence registry (useful for contract tests).",
    )
    return parser.parse_args()


def read_text(path: Path, errors: list[str]) -> str:
    try:
        return path.read_text(encoding="utf-8")
    except OSError as exc:
        errors.append(f"missing or unreadable {path}: {exc}")
        return ""


def load_json_document(path: Path, errors: list[str]) -> dict[str, object]:
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


def validate_contract(
    contract: dict[str, object], errors: list[str]
) -> tuple[int, int, set[int], set[str]]:
    target = contract.get("target_pages")
    parts = contract.get("parts")
    if not isinstance(target, dict):
        errors.append("target_pages must be an object")
        target = {}
    if not isinstance(parts, list):
        errors.append("parts must be a list")
        parts = []

    required_syntax = contract.get("required_syntax_ids")
    if not isinstance(required_syntax, list) or not all(
        isinstance(value, str) and value for value in required_syntax
    ):
        errors.append("required_syntax_ids must be a non-empty list of strings")
        required_syntax = []
    required_syntax_ids = set(required_syntax)
    if len(required_syntax_ids) != len(required_syntax):
        errors.append("required_syntax_ids contains duplicates")

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
        return len(chapters), 0, valid_numbers, required_syntax_ids

    if not minimum <= planned <= maximum:
        errors.append(f"planned pages {planned} are outside {minimum}..{maximum}")
    calculated = front + appendices + part_pages
    if calculated != planned:
        errors.append(f"planned pages are {planned}, but budgets sum to {calculated}")

    return len(chapters), planned, valid_numbers, required_syntax_ids


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


def validate_coverage(
    text: str,
    chapter_numbers: set[int],
    required_identifiers: set[str],
    errors: list[str],
) -> int:
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
        reference_fields: dict[str, set[int]] = {}
        for field_name, field_value in (
            ("first teaching", first_taught),
            ("immediate practice", practice),
            ("later reuse", reuse),
        ):
            references = {int(match) for match in CHAPTER_REFERENCE.findall(field_value)}
            reference_fields[field_name] = references
            if not references:
                errors.append(f"{identifier} has no chapter reference for {field_name}")
            invalid = references - chapter_numbers
            if invalid:
                errors.append(f"{identifier} references invalid chapters {sorted(invalid)}")
        first_references = reference_fields["first teaching"]
        practice_references = reference_fields["immediate practice"]
        if (
            first_references
            and practice_references
            and min(practice_references) < min(first_references)
        ):
            errors.append(f"{identifier}: practice precedes first teaching")
    missing_identifiers = required_identifiers - identifiers
    if missing_identifiers:
        errors.append(f"missing required syntax IDs: {sorted(missing_identifiers)}")
    return len(rows)


def validate_chapter_units(
    units: dict[str, object],
    chapter_numbers: set[int],
    calibration_chapters: set[int],
    root: Path,
    errors: list[str],
) -> int:
    entries = units.get("chapters")
    if not isinstance(entries, list):
        errors.append("chapter units must contain a chapters list")
        return 0

    numbers = [entry.get("number") for entry in entries if isinstance(entry, dict)]
    if numbers != sorted(chapter_numbers):
        errors.append(f"chapter unit numbers must match the book contract, found {numbers}")

    accepted = 0
    for entry in entries:
        if not isinstance(entry, dict):
            errors.append("each chapter unit must be an object")
            continue
        number = entry.get("number", "?")
        state = entry.get("state")
        if state not in {"planned", "draft", "accepted"}:
            errors.append(f"chapter {number} has invalid learning-unit state {state!r}")
            continue
        if state != "accepted":
            continue

        accepted += 1
        evidence = entry.get("evidence")
        if not isinstance(evidence, dict):
            errors.append(f"accepted chapter {number} is missing evidence")
            continue

        prerequisite_mode = evidence.get("prerequisite_mode")
        if prerequisite_mode is not None:
            if prerequisite_mode != "calibration-contract":
                errors.append(
                    f"accepted chapter {number} has invalid prerequisite mode "
                    f"{prerequisite_mode!r}"
                )
            if number not in calibration_chapters:
                errors.append(f"chapter {number} is not a declared calibration chapter")
            prerequisites = evidence.get("prerequisite_chapters")
            if (
                not isinstance(prerequisites, list)
                or not prerequisites
                or not all(
                    isinstance(chapter, int) and 0 < chapter < number
                    for chapter in prerequisites
                )
            ):
                errors.append(
                    f"calibration chapter {number} needs earlier prerequisite chapters"
                )

        for field in ("tex", "feedback"):
            relative = evidence.get(field)
            if not isinstance(relative, str) or not (root / relative).is_file():
                errors.append(f"accepted chapter {number} has invalid {field} evidence")

        examples = evidence.get("examples")
        if not isinstance(examples, list) or not examples or not all(
            isinstance(relative, str) and (root / relative).is_file()
            for relative in examples
        ):
            errors.append(f"accepted chapter {number} needs existing example evidence")

        failure = evidence.get("failure_experiment")
        if not isinstance(failure, dict):
            errors.append(f"accepted chapter {number} needs failure-experiment evidence")
        else:
            source = failure.get("source")
            category = failure.get("diagnostic_category")
            if not isinstance(source, str) or not (root / source).is_file():
                errors.append(f"accepted chapter {number} has invalid failure source")
            if not isinstance(category, str) or not category.strip() or PLACEHOLDER.search(category):
                errors.append(f"accepted chapter {number} has invalid diagnostic category")

        output_task = evidence.get("output_task")
        if (
            not isinstance(output_task, str)
            or not output_task.strip()
            or PLACEHOLDER.search(output_task)
        ):
            errors.append(f"accepted chapter {number} needs a concrete output task")

        self_checks = evidence.get("self_checks")
        if not isinstance(self_checks, int) or self_checks < 1:
            errors.append(f"accepted chapter {number} needs at least one self-check")

        markers = evidence.get("checklist_markers")
        marker_set = set(markers) if isinstance(markers, list) else set()
        missing_markers = CHECKLIST_MARKERS - marker_set
        if missing_markers:
            errors.append(
                f"accepted chapter {number} is missing checklist evidence: "
                f"{sorted(missing_markers)}"
            )

    return accepted


def declared_cmake_target(cmake_text: str, target: str) -> bool:
    declaration = re.compile(
        rf"\b(?:quant_target|quant_project_target|add_executable|add_library)\(\s*"
        rf"{re.escape(target)}(?:\s|\))"
    )
    return declaration.search(cmake_text) is not None


def validate_code_evidence(
    evidence: dict[str, object], root: Path, errors: list[str]
) -> int:
    listings = evidence.get("complete_listings")
    failures = evidence.get("intentional_failures")
    snapshots = evidence.get("chapter_snapshots")
    if not isinstance(listings, list):
        errors.append("code evidence must contain a complete_listings list")
        listings = []
    if not isinstance(failures, list):
        errors.append("code evidence must contain an intentional_failures list")
        failures = []
    if not isinstance(snapshots, list):
        errors.append("code evidence must contain a chapter_snapshots list")
        snapshots = []

    tex_sources: set[str] = set()
    tex_paths = [
        *sorted((root / "chapters").glob("ch*.tex")),
        *sorted((root / "appendices").glob("*.tex")),
    ]
    for tex_path in tex_paths:
        tex_sources.update(LISTING_REFERENCE.findall(read_text(tex_path, errors)))

    registered_sources = {
        entry.get("source")
        for entry in listings
        if isinstance(entry, dict) and isinstance(entry.get("source"), str)
    }
    unregistered = tex_sources - registered_sources
    stale = registered_sources - tex_sources
    if unregistered:
        errors.append(f"unregistered complete listings: {sorted(unregistered)}")
    if stale:
        errors.append(f"registered listings are not referenced by TeX: {sorted(stale)}")

    cmake_files = [root / "CMakeLists.txt", *sorted((root / "code").glob("**/CMakeLists.txt"))]
    cmake_text = "\n".join(read_text(path, errors) for path in cmake_files)
    for index, entry in enumerate(listings, start=1):
        if not isinstance(entry, dict):
            errors.append(f"complete listing {index} must be an object")
            continue
        tex = entry.get("tex")
        source = entry.get("source")
        target = entry.get("target")
        if not isinstance(tex, str) or not (root / tex).is_file():
            errors.append(f"complete listing {index} has an invalid TeX path")
        if not isinstance(source, str) or not (root / source).is_file():
            errors.append(f"complete listing {index} has an invalid source path")
        if not isinstance(target, str) or not declared_cmake_target(cmake_text, target):
            errors.append(f"complete listing {index} has an undeclared CMake target {target!r}")
        if isinstance(tex, str) and isinstance(source, str) and (root / tex).is_file():
            if source not in LISTING_REFERENCE.findall(read_text(root / tex, errors)):
                errors.append(f"{source} is not referenced by its declared TeX file {tex}")

    for index, entry in enumerate(failures, start=1):
        if not isinstance(entry, dict):
            errors.append(f"intentional failure {index} must be an object")
            continue
        source = entry.get("source")
        stage = entry.get("stage")
        category = entry.get("diagnostic_category")
        if not isinstance(source, str) or not (root / source).is_file():
            errors.append(f"intentional failure {index} has an invalid source path")
        if stage not in {"compile", "link", "runtime"}:
            errors.append(f"intentional failure {index} has an invalid stage")
        if not isinstance(category, str) or not category.strip() or PLACEHOLDER.search(category):
            errors.append(f"intentional failure {index} has an invalid diagnostic category")

    for index, entry in enumerate(snapshots, start=1):
        if not isinstance(entry, dict):
            errors.append(f"chapter snapshot {index} must be an object")
            continue
        relative = entry.get("path")
        target = entry.get("entry_target")
        snapshot = root / relative if isinstance(relative, str) else root / "__invalid__"
        snapshot_cmake = snapshot / "CMakeLists.txt"
        if not snapshot.is_dir() or not snapshot_cmake.is_file():
            errors.append(f"chapter snapshot {index} is not independently configurable")
            continue
        snapshot_text = read_text(snapshot_cmake, errors)
        if not isinstance(target, str) or not declared_cmake_target(snapshot_text, target):
            errors.append(f"chapter snapshot {index} has an undeclared entry target")

    return len(listings)


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

    contract = load_json_document(authoring / "book-contract.json", errors)
    chapter_count, planned_pages, chapter_numbers, required_syntax_ids = validate_contract(
        contract, errors
    )

    coverage_path = args.coverage.resolve() if args.coverage else authoring / "syntax-coverage.md"
    coverage_text = read_text(coverage_path, errors)
    coverage_count = validate_coverage(
        coverage_text, chapter_numbers, required_syntax_ids, errors
    )

    raw_calibration_chapters = contract.get("calibration_chapters", [])
    if not isinstance(raw_calibration_chapters, list) or not all(
        isinstance(number, int) and number in chapter_numbers
        for number in raw_calibration_chapters
    ):
        errors.append("book contract has invalid calibration chapters")
        calibration_chapters: set[int] = set()
    else:
        calibration_chapters = set(raw_calibration_chapters)

    units_path = args.units.resolve() if args.units else authoring / "chapter-units.json"
    units = load_json_document(units_path, errors)
    accepted_units = validate_chapter_units(
        units, chapter_numbers, calibration_chapters, root, errors
    )

    evidence_path = (
        args.code_evidence.resolve()
        if args.code_evidence
        else authoring / "code-evidence.json"
    )
    code_evidence = load_json_document(evidence_path, errors)
    listing_count = validate_code_evidence(code_evidence, root, errors)

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
        f"{listing_count} complete listings, {accepted_units} accepted units, "
        f"{planned_pages} planned pages)"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
