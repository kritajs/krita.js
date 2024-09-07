from krita import *
import sys
from pathlib import Path
from PyQt5.QtCore import qDebug

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

    qwindow = Krita.instance().activeWindow().qwindow()
    qwindow.destroyed.connect(dispose)
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
