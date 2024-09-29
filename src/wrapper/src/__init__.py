import ctypes
import traceback
from pathlib import Path
from PyQt5.QtCore import qDebug

try:
    # Load Ultralight and krita.js libraries
    base_path = Path(__file__).parent
    bin_path = base_path / "bin"
    ctypes.CDLL(str(bin_path / "AppCore"))
    ctypes.CDLL(str(bin_path / "Ultralight"))
    ctypes.CDLL(str(bin_path / "UltralightCore"))
    ctypes.CDLL(str(bin_path / "WebCore"))
    kritajs_lib = ctypes.CDLL(str(bin_path / "libkritajs"))

    # Initialize krita.js
    init_kritajs = kritajs_lib.initialize
    init_kritajs.argtypes = [ctypes.c_char_p]
    init_kritajs(str(base_path).encode("utf-8"))

    # Wait for the main window to be created before we start rendering
    appNotifier = Krita.instance().notifier()
    appNotifier.setActive(True)
    appNotifier.windowCreated.connect(kritajs_lib.start)
except OSError as e:
    qDebug(traceback.format_exc())
