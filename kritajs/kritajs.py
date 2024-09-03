from krita import *
import sys
from pathlib import Path
from PyQt5.QtCore import qDebug

# Add lib folder to sys.path so we can import packages from lib
p = Path(__file__).parent / "lib"
sys.path.append(str(p))

import sciter

frame = None

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
    global frame
    if (frame is not None):
        qDebug("krita.js is already running!")
        return

    qDebug("Starting krita.js...")

    # Close Sciter when main Krita window closes
    Krita.instance().activeWindow().qwindow().destroyed.connect(dispose)

    # isMain should be set to True in order for Sciter to close correctly when Krita closes.
    # Main windows terminate the Sciter process when closed.
    # If false, Sciter will still be loaded and prevent Krita from fully closing, causing memory leaks.
    frame = sciter.Window(ismain=True)
    frame.load_file(str(Path(__file__).parent / "minimal.htm"))
    frame.expand()

    # run_app() will enter the main Sciter loop which is infinite until the main Sciter window is closed
    frame.run_app()

    # Code after run_app will not run until the main Sciter window is closed
    frame = None
    qDebug("krita.js has been closed!")

def dispose():
    global frame
    if (frame is not None):
        qDebug("Closing krita.js...")
        frame.dismiss()
        frame.quit_app()
        frame = None

ext = kritajs(Krita.instance())
Krita.instance().addExtension(ext) 
