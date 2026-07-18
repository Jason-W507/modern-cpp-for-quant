from __future__ import annotations

from pathlib import Path


def is_msvc(compiler: Path) -> bool:
    return compiler.name.lower() in {"cl", "cl.exe", "clang-cl", "clang-cl.exe"}


def compile_object_command(
    compiler: Path, source: Path, object_file: Path, *, warnings: bool = False
) -> list[str]:
    if is_msvc(compiler):
        command = [
            str(compiler),
            "/nologo",
            "/std:c++20",
        ]
        if warnings:
            command.append("/W4")
        return [*command, "/c", str(source), f"/Fo{object_file}"]

    command = [
        str(compiler),
        "-std=c++20",
    ]
    if warnings:
        command.extend(["-Wall", "-Wextra", "-Wpedantic"])
    return [*command, "-c", str(source), "-o", str(object_file)]


def link_executable_command(
    compiler: Path, object_file: Path, executable: Path
) -> list[str]:
    return link_objects_command(compiler, [object_file], executable)


def link_objects_command(
    compiler: Path, object_files: list[Path], executable: Path
) -> list[str]:
    objects = [str(object_file) for object_file in object_files]
    if is_msvc(compiler):
        return [str(compiler), "/nologo", *objects, f"/Fe{executable}"]
    return [str(compiler), *objects, "-o", str(executable)]


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
