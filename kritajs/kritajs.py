from krita import *
import sys
from pathlib import Path
from PyQt5.QtCore import qDebug

# Add lib folder to sys.path so we can import packages from lib
p = Path(__file__).parent / "lib"
sys.path.append(str(p))

import sciter

class kritajs(Extension):

    def __init__(self, parent):
        # This is initialising the parent, always important when subclassing.
        super().__init__(parent)

    def setup(self):
        # This runs only once when app is installed
        pass

    def createActions(self, window):
        action = window.createAction("runKritajs", "Run krita.js", "tools/scripts/krita.js")
        action.triggered.connect(self.start)

    def start(self):
        frame = sciter.Window(ismain=True, uni_theme=True)
        frame.load_file(str(Path(__file__).parent / "minimal.htm"))
        frame.expand()
        frame.run_app()

# And add the extension to Krita's list of extensions:
Krita.instance().addExtension(kritajs(Krita.instance())) 
