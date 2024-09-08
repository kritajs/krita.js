# krita.js

krita.js is a Krita plugin that allows users to create plugins using HTML, CSS and JavaScript.

## To do

- [ ] Implement Ultralight prototype
- [ ] Draw Ultralight view into QDockWidget
- [ ] Create JS bindings for Krita API
- [ ] Generate .d.ts files for Krita API
- [ ] Figure out interface for krita.js plugins
- [ ] Set up packaging/release pipeline for multiple platforms

## Developing krita.js

This section is for people developing the krita.js plugin itself. Note that these instructions have only been tested on Windows (64-bit) and using VS Code.

### Prerequisites

To build krita.js, you'll need to download/install the following:

- [Krita](https://krita.org/en/download/)
  - Must be installed so that the compiler can link to the `libkis` and Qt libraries
  - krita.js targets Krita 5.2.3
  - Needs to be installed to `C:/Program Files/Krita (x64)`
- [LLVM MinGW compiler toolchain](https://github.com/mstorsjo/llvm-mingw/releases/download/20220906/llvm-mingw-20220906-ucrt-x86_64.zip)
  - This is the same toolchain used to build Krita. We use the same exact version as the one Krita uses to ensure maximum compatibility when linking to `libkis` (Krita's API) and Qt.
  - Extract the archive to any destination
  - Add LLVM MinGW's `bin` folder to your `PATH`. You can confirm this is working by opening a shell and running `g++ --version`.

You will also need a few code dependencies. Each of the dependencies should be placed in a `deps` folder at the root of this repo.

- [Krita source](https://invent.kde.org/graphics/krita)
  - Contains the `libkis` headers which will allow krita.js to call into the `libkis` dynamic library
- [qtbase](https://mirrors.dotsrc.org/.mirrors/qtproject/online/qtsdkrepository/windows_x86/desktop/qt5_5152/qt.qt5.5152.win64_mingw81/5.15.2-0-202011130602qtbase-Windows-Windows_10-Mingw-Windows-Windows_10-X86_64.7z)
  - Contains headers for the main functionality of Qt
  - After extracting, rename the `mingw81_64` folder to `qtbase-5.15.2-mingw81_64` and then move it to `deps`

Your folder structure should look like:

```
krita.js/
├─ .vscode/
├─ deps/
│  ├─ krita/
│  ├─ qtbase-5.15.2-mingw81_64/
├─ src/
├─ typings/
```

### Building

In VS Code, press **Ctrl + Shift + B** to run the build.

### Using symlinks

When developing krita.js, it's useful to create symlinks from the Krita plugin folder to the built plugin. This removes the need to copy the plugin to the Krita plugin folder every time you build.

To create symlinks on Windows, clone this repo and then run the commands below in a command prompt (not PowerShell). You may need to run the command prompt as an administrator.

Create symlink to `kritajs` folder:

```
mklink /D "C:\Users\myuser\AppData\Roaming\krita\pykrita\kritajs" "absolute\path\to\krita.js\src\plugin\kritajs"
```

Create symlink to `kritajs.desktop` file:

```
mklink /H "C:\Users\myuser\AppData\Roaming\krita\pykrita\kritajs.desktop" "absolute\path\to\krita.js\src\plugin\kritajs.desktop"
```

### Running krita.js

If you haven't already, enable krita.js in the [Python Plugin Manager](https://docs.krita.org/en/reference_manual/preferences/python_plugin_manager.html).

You can run krita.js by selecting **Tools > Scripts > Run krita.js**.
