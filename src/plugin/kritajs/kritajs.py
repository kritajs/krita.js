from krita import *
import ctypes
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

    lib_path = Path(__file__).parent / "bin/loader.dll"
    lib = ctypes.CDLL(str(lib_path))
    py_add_one = lib.add_one
    py_add_one.argtypes = [ctypes.c_int]
    value = 5
    results = py_add_one(value)
    qDebug(str(results))

def dispose():
    pass

ext = kritajs(Krita.instance())
Krita.instance().addExtension(ext)
