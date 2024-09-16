from krita import *
import ctypes
import traceback
from pathlib import Path
from PyQt5.QtCore import qDebug

class kritajs(Extension):
    def __init__(self, parent):
        super().__init__(parent)

    # This runs only once when plugin is installed
    def setup(self):
        pass

    def createActions(self, window):
        action = window.createAction("runKritajs", "Run krita.js", "tools/scripts/krita.js")
        action.triggered.connect(start)

def start():
    # Perform any cleanup when Krita needs to close
    qwindow = Krita.instance().activeWindow().qwindow()
    qwindow.destroyed.connect(dispose)

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
        init_kritajs = kritajs_lib.KRITAJS
        init_kritajs.argtypes = [ctypes.c_char_p]
        init_kritajs(str(base_path).encode("utf-8"))
    except OSError as e:
        qDebug(traceback.format_exc())

def dispose():
    pass

ext = kritajs(Krita.instance())
Krita.instance().addExtension(ext)
