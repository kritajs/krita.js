import argparse
import glob
import shutil
from pathlib import Path

from generate_ts import generate_ts
from parse import parse, webrepr
from preprocess import preprocess
from utils import write_file
from cxxheaderparser.simple import ClassScope
from Class import Class

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("krita_dir", help="Path to Krita repo")
    parser.add_argument("-o", "--output_dir", help="Path to output generated files", required=True)
    args = parser.parse_args()
    krita_dir = Path(args.krita_dir)
    output_dir = Path(args.output_dir)
    print("Krita source directory: " + str(krita_dir))
    print("Output directory: " + str(output_dir))

    # Process all libkis headers
    headers = glob.glob("libs/libkis/*.h", root_dir=str(krita_dir))
    headers += [
        # DockWidget inherits from this
        "libs/flake/KoCanvasObserverBase.h",
        # DockWidgetFactoryBase inherits from this
        "libs/flake/KoDockFactoryBase.h"
    ]
    ignore = [
        # Not used by anything
        "LibKisUtils.h",
        # Don't think we need to expose this to JS.
        # It also depends on boost and other non-libkis headers
        "PresetChooser.h"
    ]
    headers = filter(lambda h: Path(h).name not in ignore, headers)
    classNames = []
    for header in headers:
        file_path = krita_dir / header
        if file_path.name == "libkis.h":
            shutil.copy2(file_path, output_dir)
            return
        source_code = file_path.read_text()
        preprocessed = preprocess(source_code) 
        parsed_data = parse(preprocessed)
        base_class_includes: set[str] = set()
        for c in parsed_data.namespace.classes:
            (output, _base_class_includes) = Class(c)
            base_class_includes = _base_class_includes
            classNames.append(output)
            print(output)
        

    # with the headers, first construct a list that will get all of the exported type first
    # header = headers[0]
    # # libkis.h is a common file included by most of the other libkis headers.
    # # It contains a bunch of Qt includes and forward declarations of other libkis
    # # classes. We don't need to parse it -- just copy it as-is.

    # print("Generating files for " + str(file_path) + "...")
    # source_code = file_path.read_text()

    # # cxxheaderparser can't parse certain preprocessor directives so we preprocess first
    # preprocessed = preprocess(source_code) 
    # parsed_data = parse(preprocessed)
    # generated_header = generate_ts(parsed_data)

    # # write_file(output_dir / file_path.name.replace(".h", "_pp.h"), preprocessed)
    # # write_file(output_dir / (file_path.name + ".r"), webrepr(parsed_data))
    # write_file(output_dir / (file_path.name + ".d.ts"), generated_header)

main()
