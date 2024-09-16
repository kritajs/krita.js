import argparse
from pathlib import Path

from generator import Generator
from generators.header.header_generator import HeaderGenerator
from generators.typescript.typescript_generator import TypeScriptGenerator
from parse import parse
from preprocess import preprocess

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("krita_dir", help="Path to Krita repo")
    parser.add_argument("-o", "--output_dir", help="Path to output generated files", required=True)
    args = parser.parse_args()
    krita_dir = Path(args.krita_dir)
    output_dir = Path(args.output_dir)
    print("Krita source directory: " + str(krita_dir))
    print("Output directory: " + str(output_dir))

    generators: list[Generator] = [
        HeaderGenerator(),
        # TypeScriptGenerator()
    ]

    for generator in generators:
        file_paths = generator.get_file_paths(krita_dir)
        for file_path in file_paths:
            full_path = krita_dir / file_path
            source_code = full_path.read_text()
            # cxxheaderparser can't parse certain preprocessor directives so we preprocess first
            preprocessed = preprocess(source_code) 
            parsed_data = parse(preprocessed)
            generator.process(full_path, output_dir, parsed_data)

main()
