import sys
from pathlib import Path

# Add lib folder to sys.path so we can import packages from lib
p = Path(__file__).parent.parent / "lib"
sys.path.append(str(p))

import sciter

if __name__ == '__main__':
    frame = sciter.Window(ismain=True, uni_theme=True)
    frame.load_file(str(Path(__file__).parent / "minimal.htm"))
    frame.expand()
    frame.run_app()
