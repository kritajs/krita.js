> PROJECT IS NOT ACTIVELY BEING DEVELOPED ANYMORE

# krita.js

krita.js is a Krita plugin that allows users to create plugins using HTML, CSS and JavaScript.

## To do

- [ ] Create JS bindings for Krita API
- [ ] Generate .d.ts files for Krita API
- [ ] Figure out interface for krita.js plugins
- [ ] Set up packaging/release pipeline for multiple platforms
- [ ] Add dev tools

Qt-specific:

- [ ] Expose Qt classes to JS
- [ ] Integrate Qt signals/slots
- [ ] Allow JS to extend Qt classes

## Developing krita.js

This section is for people developing the krita.js plugin itself. Note that these instructions have only been tested on Windows (64-bit).

### Prerequisites

To build krita.js, you'll need to download/install the following:

- [Krita](https://krita.org/en/download/)
  - Must be installed so that the linker can link to `libkis`
  - krita.js targets Krita 5.2.6
  - Needs to be installed to `C:/Program Files/Krita (x64)`
- [Node.js](https://nodejs.org/en)
- [CMake](https://cmake.org/)
- [LLVM MinGW compiler toolchain](https://github.com/mstorsjo/llvm-mingw/releases/download/20220906/llvm-mingw-20220906-ucrt-x86_64.zip)
  - This is the same toolchain used to build Krita. We use the same exact version as the one Krita uses to ensure maximum compatibility when linking to Krita's libraries.
  - Extract the archive to any destination
  - Add LLVM MinGW's `bin` folder to your `PATH`. You can confirm this is working by opening a shell and running `g++ --version`.

Next, install the project's dependencies:

```sh
npm install
```

This will automatically download and install the following dependencies into `packages/deps/installed`:

- [Krita source](https://invent.kde.org/graphics/krita)
- [Qt](https://invent.kde.org/dkazakov/krita-ci-artifacts-windows-qt5.15/-/packages/465411)
- [Ultralight](https://ultralig.ht/)

The total download size is a few hundred megabytes so this step may take a bit of time.

### Building

First, generate the CMake project by running:

```sh
npm run prebuild
```

> If you change the `CMakeLists.txt` file, you must run this command again to regenerate the project.

Run the following command to build:

```sh
npm run build
```

This will build and package krita.js into the `build_package` directory. To test the plugin, copy the contents of this directory into your Krita Python plugins directory.

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
