# krita.js

krita.js is a Krita plugin that allows users to create plugins using HTML, CSS and JavaScript.

## To do

- [x] Implement Ultralight prototype
- [x] Draw Ultralight view into QDockWidget
- [x] Figure out how to call `libkis`
- [x] Build krita.js as a Qt C++ plugin
- [ ] Create JS bindings for Krita API
- [ ] Generate .d.ts files for Krita API
- [ ] Figure out interface for krita.js plugins
- [ ] Set up packaging/release pipeline for multiple platforms

## Developing krita.js

This section is for people developing the krita.js plugin itself. Note that these instructions have only been tested on Windows (64-bit).

### Prerequisites

To build krita.js, you'll need to download/install the following:

- [Krita](https://krita.org/en/download/)
  - Must be installed so that the linker can link to `libkis`
  - krita.js targets Krita 5.2.3
  - Needs to be installed to `C:/Program Files/Krita (x64)`
- [CMake](https://cmake.org/download/)
- [LLVM MinGW compiler toolchain](https://github.com/mstorsjo/llvm-mingw/releases/download/20220906/llvm-mingw-20220906-ucrt-x86_64.zip)
  - This is the same toolchain used to build Krita. We use the same exact version as the one Krita uses to ensure maximum compatibility when linking to Krita's libraries.
  - Extract the archive to any destination
  - Add LLVM MinGW's `bin` folder to your `PATH`. You can confirm this is working by opening a shell and running `g++ --version`.

You will also need a few code dependencies. Each of the dependencies should be placed in a `deps` folder at the root of this repo.

- [Krita source](https://invent.kde.org/graphics/krita) - check out tag v5.2.3, which is the latest version of Krita at the time of writing
- [Qt](https://invent.kde.org/dkazakov/krita-ci-artifacts-windows-qt5.15/-/package_files/930824/download) - rename the extracted folder to `qt`
- [Ultralight 1.3.0](https://ultralight-files.sfo3.cdn.digitaloceanspaces.com/ultralight-sdk-1.3.0-win-x64.7z)

Your folder structure should look like:

```
krita.js/
├─ .vscode/
├─ deps/
│  ├─ krita/
│  ├─ qt/
│  ├─ ultralight-sdk-1.3.0-win-x64/
├─ src/
├─ tools/
├─ typings/
```

### Building

First, you will need to generate the CMake project. To do this, run:

```sh
cmake -B build -G "MinGW Makefiles"
```

> If you change the `CMakeLists.txt` file, you must run this command again to regenerate the project.

Run the following command to build:

```sh
cmake --build build --target install
```

### Using symlinks

When developing krita.js, it's useful to create symlinks from the Krita plugin folder to the built plugin. This removes the need to copy the plugin to the Krita plugin folder every time you build.

To create symlinks on Windows, run the commands below in a command prompt (not PowerShell). You may need to run the command prompt as an administrator.

Create symlink to `kritajs` folder:

```
mklink /D "C:\Users\myuser\AppData\Roaming\krita\pykrita\kritajs" "absolute\path\to\krita.js\build_package\kritajs"
```

Create symlink to `kritajs.desktop` file:

```
mklink /H "C:\Users\myuser\AppData\Roaming\krita\pykrita\kritajs.desktop" "absolute\path\to\krita.js\build_package\kritajs.desktop"
```
