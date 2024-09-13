import argparse
import glob
import shutil
from pathlib import Path

from generate_ts import generate_ts
from parse import parse, webrepr
from preprocess import preprocess
from utils import write_file
from cxxheaderparser.simple import ClassScope
from Class import Class, getPlainClassName, get_ts_ready_class

# c++ type: [ts type, path to import from if any]
type_matching_dict = {
    "qreal":        ["number",  ""],
    "bool":         ["boolean", ""],
    "void":         ["void",    ""],
    "qint8":        ["number",  ""],
    "qint16":       ["number",  ""],
    "qint32":       ["number",  ""],
    "qint64":       ["number",  ""],
    "qintptr":      ["number",  ""],
    "qlonglong":    ["number",  ""],
    "qptrdiff":     ["number",  ""],
    "qsizetype":    ["number",  ""],
    "quint8" :      ["number",  ""],
    "quint16":      ["number",  ""],
    "quint32":      ["number",  ""],
    "quint64":      ["number",  ""],
    "quintptr":     ["number",  ""],
    "qulonglong":   ["number",  ""],
    "uchar":        ["string",  ""],
    "uint":         ["number",  ""],
    "ulong":        ["number",  ""],
    "ushort":       ["number",  ""]
}

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
    i = 0
    headers = filter(lambda h: Path(h).name not in ignore, headers)
    for header in headers:
        file_path = krita_dir / header
        if file_path.name == "libkis.h":
            shutil.copy2(file_path, output_dir)
            continue
        source_code = file_path.read_text()
        preprocessed = preprocess(source_code) 
        parsed_data = parse(preprocessed)
        for c in parsed_data.namespace.classes:
            # className = getPlainClassName(c)
            # we add in types that we can use which are from the same dir
            # type_matching_dict[className] = className
            get_ts_ready_class(c)

    # # write_file(output_dir / file_path.name.replace(".h", "_pp.h"), preprocessed)
    # # write_file(output_dir / (file_path.name + ".r"), webrepr(parsed_data))
    # write_file(output_dir / (file_path.name + ".d.ts"), generated_header)

main()
