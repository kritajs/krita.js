from pathlib import Path

def write_file(path: Path, content: str):
    path.parent.mkdir(exist_ok=True, parents=True)
    path.write_text(content)

def delete_file(path: Path):
    path.unlink()
