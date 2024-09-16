from abc import ABC, abstractmethod
from cxxheaderparser.simple import ParsedData
from pathlib import Path

class Generator(ABC):
    # Called once by bindgen to get a path list of files this generator will process.
    # Paths returned should be relative to krita_dir (path to the Krita repo).
    @abstractmethod
    def get_file_paths(self, krita_dir: Path) -> list[str]:
        pass

    # Process the given parsed source file
    @abstractmethod
    def process(self, file_path: Path, output_dir: Path, data: ParsedData):
        pass
