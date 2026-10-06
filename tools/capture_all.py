"""Снимки создаются настоящими Qt-приложениями после их сборки."""
from pathlib import Path
import argparse
import os
import platform
import subprocess

ROOT = Path(__file__).resolve().parents[1]

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--build', type=Path, required=True, help='Папка сборки CMake')
    parser.add_argument('--label', choices=['windows', 'ubuntu', 'linux', 'macos'], required=True)
    args = parser.parse_args()
    current = platform.system()
    if (args.label == 'windows' and current != 'Windows') or (args.label == 'macos' and current != 'Darwin'):
        parser.error('Метка ОС не соответствует текущей системе.')
    if args.label in ('linux', 'ubuntu') and current != 'Linux':
        parser.error('Метка Linux не соответствует текущей системе.')
    if args.label == 'ubuntu' and 'ID=ubuntu' not in Path('/etc/os-release').read_text():
        parser.error('Метка ubuntu допускается только при запуске в Ubuntu.')
    out = ROOT / 'reports' / 'screenshots' / args.label
    out.mkdir(parents=True, exist_ok=True)
    suffix = '.exe' if os.name == 'nt' else ''
    jobs = [('Calculator', 'lab1', ['--demo']), ('Alarm', 'lab2', []),
            ('handbook_lab3', 'lab3', []), ('handbook', 'lab4', []),
            ('StudyPlanner', 'lab5_tasks', ['--demo', '--tab', '0']),
            ('StudyPlanner', 'lab5_sessions', ['--demo', '--tab', '1']),
            ('StudyPlanner', 'lab5_summary', ['--demo', '--tab', '2'])]
    for name, image, extra in jobs:
        candidates = [args.build / 'bin' / (name + suffix), args.build / 'bin' / 'Release' / (name + suffix),
                      args.build / 'bin' / 'Debug' / (name + suffix)]
        exe = next((p for p in candidates if p.is_file()), None)
        if exe is None:
            raise FileNotFoundError(f'Не найден {name}. Сначала соберите корневой проект.')
        path = out / (image + '.png')
        environment = os.environ.copy()
        if args.label == 'windows':
            environment['QT_QPA_PLATFORM'] = 'windows:darkmode=0'
        subprocess.run([str(exe.resolve()), *extra, '--screenshot', str(path)],
                       env=environment, check=True, timeout=30)
        if not path.is_file() or path.stat().st_size == 0:
            raise RuntimeError(f'Снимок не создан: {path}')
        print(path)
    (out / 'environment.txt').write_text(platform.platform() + '\n', encoding='utf-8')

if __name__ == '__main__':
    main()
