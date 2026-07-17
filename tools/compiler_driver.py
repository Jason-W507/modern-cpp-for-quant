from __future__ import annotations

from pathlib import Path


def is_msvc(compiler: Path) -> bool:
    return compiler.name.lower() in {"cl", "cl.exe", "clang-cl", "clang-cl.exe"}


def compile_object_command(
    compiler: Path, source: Path, object_file: Path
) -> list[str]:
    if is_msvc(compiler):
        return [
            str(compiler),
            "/nologo",
            "/std:c++20",
            "/c",
            str(source),
            f"/Fo{object_file}",
        ]
    return [str(compiler), "-std=c++20", "-c", str(source), "-o", str(object_file)]


def link_executable_command(
    compiler: Path, object_file: Path, executable: Path
) -> list[str]:
    if is_msvc(compiler):
        return [str(compiler), "/nologo", str(object_file), f"/Fe{executable}"]
    return [str(compiler), str(object_file), "-o", str(executable)]


def compile_executable_command(
    compiler: Path, source: Path, executable: Path
) -> list[str]:
    if is_msvc(compiler):
        return [
            str(compiler),
            "/nologo",
            "/std:c++20",
            str(source),
            f"/Fe{executable}",
        ]
    return [str(compiler), "-std=c++20", str(source), "-o", str(executable)]
