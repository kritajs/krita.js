import argparse
import glob
import re
import shutil
from pathlib import Path

from generate_ts import generate_ts
from ts_class import create_ts_ready_class

import sys
parent_dir = str(Path().resolve().parents[0])
sys.path.insert(0, parent_dir)

from bindgen.src.parse import parse
from bindgen.src.preprocess import preprocess
from bindgen.src.utils import write_file

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
    "ushort":       ["number",  ""],
    "int":          ["number",  ""],
    "long":         ["number",  ""],
    "float":        ["number",  ""],
    "double":       ["number",  ""],
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

    headers = list(filter(lambda h: Path(h).name not in ignore, headers))
    
    def add_library_class_to_type_matching_dict(header_file_path):
        class_type = Path(header_file_path).stem
        type_matching_dict[class_type] = [class_type, class_type]

    for header in headers:
        add_library_class_to_type_matching_dict(header)

    for header in headers:
        file_path = krita_dir / header
        if file_path.name == "libkis.h":
            shutil.copy2(file_path, output_dir)
            continue
        source_code = file_path.read_text()
        preprocessed = preprocess(source_code) 
        parsed_data = parse(preprocessed)
        
        print("Generating typescript type declaration file for " + str(file_path) + "...")

        # pass type_matching_dict and header list for the generate ts
        ts_class = create_ts_ready_class(parsed_data.namespace.classes[0], file_path.name)
        generated_ts = generate_ts(ts_class, type_matching_dict)
        write_file(output_dir / (ts_class.file_name.replace('.h', '') + ".d.ts"), generated_ts)

main()
