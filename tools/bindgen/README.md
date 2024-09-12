# bindgen

bindgen is a Python tool for generating:

- Custom headers for `libkis` to allow krita.js to call `libkis` from C++. These headers do a few things:
  - Allow us to purposely ignore any third-party dependencies and problematic includes such as `KoColorSpaceMaths.h` and CMake's auto-generated `kritalibkis_export.h`. See [this PR](https://github.com/tommyquant/krita.js/pull/4) for more info.
  - Simplify which headers we actually need to include. Some classes can be forward declared which means we don't need to include them.
- `libkis` to JavaScript bindings
- `libkis` TypeScript types

bindgen uses [pcpp](https://github.com/ned14/pcpp) for preprocessing and [cxxheaderparser](https://github.com/robotpy/cxxheaderparser) to parse headers. bindgen then uses the parsed output to generate the above.

## Getting started

First, you'll need the following:

- [Krita source](https://invent.kde.org/graphics/krita) - clone this into `krita.js/deps/krita`
- [Python](https://www.python.org/)

Next, run the following commands:

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

To run bindgen, use the **Run bindgen** task in VS Code.

## Developing bindgen

If you add a dependency, please update `requirements.txt` by running:

```sh
pip3 freeze > requirements.txt
```
