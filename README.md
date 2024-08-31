# krita.js

krita.js is a Krita plugin that allows users to write scripts using JavaScript.

## Compiling (Windows)

First, you will need to compile the krita.js DLL. To do this, open your start menu and open **Visual Studio <version>/x64 Native Tools Command Prompt for VS <version>**. If you are using 32-bit Krita, use the 32-bit command prompt instead.

Once the command prompt is open, change your working directory to this repo. To compile a DLL, run `cl /LD path/to/kritajs.c`.

Afterwards, open Krita and open **Tools > Script > Scripter**. Open `test.py` in the scripter and modify the path to point to your compiled DLL. Finally, press the play button to run it.
