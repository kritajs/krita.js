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
    parser.add_argument("libkis_dir", help="Path to libkis headers")
    parser.add_argument("-o", "--output_dir", help="Path to output generated files", required=True)
    args = parser.parse_args()
    libkis_dir = Path(args.libkis_dir)
    output_dir = Path(args.output_dir)
    print("libkis directory: " + str(libkis_dir))
    print("Output directory: " + str(output_dir))

    # Process all libkis headers
    headers = glob.glob("*.h", root_dir=str(libkis_dir))
    for file_name in headers:
        # libkis.h is a common file included by most of the other libkis headers.
        # It contains a bunch of Qt includes and forward declarations of other libkis
        # classes. We don't need to parse it -- just copy it as-is.
        if file_name == "libkis.h":
            shutil.copy2(str(libkis_dir / file_name), output_dir)
            continue

        print("Generating files for " + file_name + "...")
        file_path = libkis_dir / file_name
        source_code = file_path.read_text()

        # cxxheaderparser can't parse certain preprocessor directives so we preprocess first
        preprocessed = preprocess(file_name, source_code) 
        parsed_data = parse(preprocessed)
        header = generate_header(parsed_data)

        # write_file(output_dir / file_name.replace(".h", "_pp.h"), preprocessed)
        # write_file(output_dir / (file_name + ".r"), webrepr(parsed_data))
        write_file(output_dir / file_name, header)

main()
