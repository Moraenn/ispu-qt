"""Скопировать дополнительные DLL зависимостей сборки MSYS2 в готовый комплект."""
import argparse
import re
import shutil
import subprocess
from pathlib import Path

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--directory', type=Path, required=True)
    parser.add_argument('--toolchain', type=Path, required=True)
    args = parser.parse_args()
    binaries = args.toolchain / 'bin'
    available = {p.name.lower(): p for p in binaries.glob('*.dll')}
    pending = list(args.directory.rglob('*.dll')) + list(args.directory.glob('*.exe'))
    processed = set()
    while pending:
        file = pending.pop()
        if file.name.lower() in processed: continue
        processed.add(file.name.lower())
        result = subprocess.run([str(binaries/'objdump.exe'), '-p', str(file)],
                                check=True, capture_output=True, text=True)
        for name in re.findall(r'DLL Name:\s*(\S+)', result.stdout):
            source = available.get(name.lower())
            if source is None: continue  # Системные библиотеки предоставляются Windows.
            target = args.directory / source.name
            if not target.exists():
                shutil.copy2(source, target)
                print(source.name)
            if name.lower() not in processed: pending.append(target)
    license_source = args.toolchain / 'share' / 'licenses'
    licenses = args.directory / 'licenses'
    for source in license_source.iterdir():
        if source.is_dir() and any(x in source.name for x in ('qt6-base','gcc-libs','icu','pcre2','harfbuzz','freetype','libpng','libjpeg','double-conversion','zstd','zlib','brotli','glib2','graphite','libb2','md4c')):
            shutil.copytree(source, licenses/source.name, dirs_exist_ok=True)

if __name__ == '__main__': main()
