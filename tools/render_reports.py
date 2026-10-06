"""Рендеринг страниц штатным render_docx.py с PDF, экспортированным Microsoft Word.

В Windows комплект Codex не содержит LibreOffice. Word используется только
для чтения и экспорта PDF; исходные DOCX не пересохраняются.
"""
import argparse
import importlib.util
import shutil
from pathlib import Path

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--renderer', type=Path, required=True)
    parser.add_argument('--documents', type=Path, required=True)
    parser.add_argument('--pdfs', type=Path, required=True)
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--dpi', type=int, default=110)
    args = parser.parse_args()
    spec = importlib.util.spec_from_file_location('render_docx', args.renderer)
    renderer = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(renderer)
    def word_pdf(doc_path, user_profile, convert_tmp_dir, stem, verbose):
        source = args.pdfs / (stem + '.pdf')
        if not source.is_file(): raise FileNotFoundError(source)
        destination = Path(convert_tmp_dir) / source.name
        shutil.copy2(source, destination)
        return str(destination), 'PDF exported read-only by Microsoft Word'
    renderer.convert_to_pdf = word_pdf
    for doc in sorted(args.documents.glob('*.docx')):
        pages = renderer.rasterize(str(doc.resolve()), str((args.output/doc.stem).resolve()),
                                   args.dpi, verbose=False, emit_pdf=True)
        print(f'{doc.name}: {len(pages)} rendered pages', flush=True)

if __name__ == '__main__': main()
