"""Package final sources, reports, evidence and the portable Windows applications."""
import argparse
import hashlib
import json
from pathlib import Path
import zipfile


DIRECTORIES = {
    "Calculator", "Alarm", "handbook", "handbook_lab3", "StudyPlanner",
    "common", "tests", "tools", "examples", "reports", "verification", "dist",
}
ROOT_FILES = {
    ".gitignore", "CMakeLists.txt", "README.md", "CHANGES.md", "DEFENSE.md",
    "REPORT_STYLE.md",
}


def shipped(path, root):
    rel = path.relative_to(root)
    if len(rel.parts) == 1:
        return rel.name in ROOT_FILES
    if rel.parts[0] not in DIRECTORIES:
        return False
    if any(p in {".git", "__pycache__"} or p.startswith("build")
           for p in rel.parts[:-1]):
        return False
    if rel.parts[:2] == ("verification", "qa"):
        return False
    return not (path.suffix == ".pyc" or path.name.startswith("~$"))


def digest(path):
    h = hashlib.sha256()
    with path.open("rb") as stream:
        for block in iter(lambda: stream.read(1024 * 1024), b""):
            h.update(block)
    return h.hexdigest()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--root", type=Path, default=Path(__file__).resolve().parents[1])
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    root = args.root.resolve()
    output = args.output.resolve()
    if output.is_relative_to(root):
        parser.error("Output archive must be outside the source directory")
    files = sorted(p for p in root.rglob("*") if p.is_file() and shipped(p, root))
    rels = {p.relative_to(root).as_posix() for p in files}
    assert len([n for n in rels if n.startswith("reports/docx/") and n.endswith(".docx")]) == 6
    for platform in ("windows", "ubuntu"):
        assert len([n for n in rels if n.startswith(f"reports/screenshots/{platform}/") and n.endswith(".png")]) == 12
    for name in ("Calculator", "Alarm", "handbook_lab3", "handbook", "StudyPlanner"):
        assert f"dist/windows/{name}.exe" in rels
    assert any(n.startswith("dist/windows/licenses/") for n in rels)

    hashes = {p.relative_to(root).as_posix(): digest(p) for p in files}
    manifest = root / "SHA256SUMS.txt"
    manifest.write_text("".join(f"{value}  {name}\n" for name, value in hashes.items()), encoding="utf-8")
    output.parent.mkdir(parents=True, exist_ok=True)
    with zipfile.ZipFile(output, "w", compression=zipfile.ZIP_DEFLATED, compresslevel=6) as archive:
        for path in files + [manifest]:
            archive.write(path, "ispu-qt-completed/" + path.relative_to(root).as_posix())
    with zipfile.ZipFile(output) as archive:
        assert archive.testzip() is None
        for name, expected in hashes.items():
            with archive.open("ispu-qt-completed/" + name) as stream:
                h = hashlib.sha256()
                for block in iter(lambda: stream.read(1024 * 1024), b""):
                    h.update(block)
            assert h.hexdigest() == expected, name
    output_hash = digest(output)
    output.with_suffix(output.suffix + ".sha256").write_text(f"{output_hash}  {output.name}\n", encoding="utf-8")
    print(json.dumps({"archive": str(output), "files": len(files) + 1,
                      "bytes": output.stat().st_size, "sha256": output_hash}, ensure_ascii=False))


if __name__ == "__main__":
    main()
