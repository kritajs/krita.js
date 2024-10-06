# bindgen

bindgen is a Python tool for generating:

- Custom headers for `libkis` to allow krita.js to call `libkis` from C++. These headers do a few things:
  - Allow us to purposely ignore any third-party dependencies and problematic includes such as `KoColorSpaceMaths.h` and CMake's auto-generated `kritalibkis_export.h`. See [this PR](https://github.com/tommyquant/krita.js/pull/4) for more info.
  - Simplify which headers we actually need to include. Bindgen forward declares classes where possible instead of including which reduces nested includes and their dependencies.
  - Removes Qt macros so that we don't need to use `moc` (a Qt build tool) to compile the headers.
- `libkis` to JavaScript bindings
- `libkis` TypeScript types
  - It generates a counterpart in `d.ts` file format.
  - NOTE: while we are working on Javascript binding, we have yet to test the type files. Please bear with us in the meantime. It also does not add import statements.
  - This tool assumes that - (1) each `.h` file contains only a single class; (2) constructors are unnecessary

bindgen uses [pcpp](https://github.com/ned14/pcpp) for preprocessing and [cxxheaderparser](https://github.com/robotpy/cxxheaderparser) to parse headers. bindgen then uses the parsed output to generate the above.

## Getting started

Run the following commands to install bindgen's dependencies into a virtual environment:

```sh
# Create a Python virtual environment
python -m venv ENV --upgrade-deps

# Activate the virtual environment
# (if using PowerShell)
ENV\Scripts\Activate.ps1
# (if using cmd)
ENV\Scripts\activate.bat

# Install dependencies
pip install -r requirements.txt
```

You can run bindgen with the following command:

```sh
# -g flag options:
# - `header`
# - `typescript`
python src/bindgen.py /path/to/krita/repo -o /output/path -g header
```

## Developing bindgen

If you add a dependency, please update `requirements.txt` by running:

```sh
pip3 freeze > requirements.txt
```
