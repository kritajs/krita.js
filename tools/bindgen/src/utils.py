from pathlib import Path

GENERATED_FOLDER_PATH = Path(__file__).parent.parent / 'generated'

def write_file(path: Path, content: str):
    path.parent.mkdir(exist_ok=True, parents=True)
    path.write_text(content)
