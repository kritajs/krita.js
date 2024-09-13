import argparse
import glob
import shutil
from pathlib import Path

from generate_header import generate_header
from parse import parse, webrepr
from preprocess import preprocess
from utils import write_file

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
    for header in headers:
        file_path = krita_dir / header
        # libkis.h is a common file included by most of the other libkis headers.
        # It contains a bunch of Qt includes and forward declarations of other libkis
        # classes. We don't need to parse it -- just copy it as-is.
        if file_path.name == "libkis.h":
            shutil.copy2(file_path, output_dir)
            continue

        print("Generating files for " + str(file_path) + "...")
        source_code = file_path.read_text()

        # cxxheaderparser can't parse certain preprocessor directives so we preprocess first
        preprocessed = preprocess(source_code) 
        parsed_data = parse(preprocessed)
        generated_header = generate_header(parsed_data)

        # write_file(output_dir / file_path.name.replace(".h", "_pp.h"), preprocessed)
        # write_file(output_dir / (file_path.name + ".r"), webrepr(parsed_data))
        write_file(output_dir / file_path.name, generated_header)

main()
