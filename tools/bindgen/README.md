# bindgen

bindgen is a Python tool for generating:

- Custom headers for `libkis` to allow krita.js to call `libkis` from C++. These headers allow us to purposely ignore any third-party dependencies and problematic includes (such as `KoColorSpaceMaths.h`). See [this PR](https://github.com/tommyquant/krita.js/pull/4) for more info.
- `libkis` to JavaScript bindings
- `libkis` TypeScript types

bindgen uses [cxxheaderparser](https://github.com/robotpy/cxxheaderparser) to parse header files. bindgen then uses the parsed output to generate the above.

## Getting started

```sh
# Create a Python virtual environment
python -m venv ENV --upgrade-deps

# Activate the virtual environment (if using PowerShell)
ENV\Scripts\Activate.ps1
# OR Activate the virtual environment (if using cmd)
ENV\Scripts\activate.bat

# Install dependencies
pip install -r requirements.txt
```
