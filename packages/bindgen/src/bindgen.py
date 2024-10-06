import argparse
from pathlib import Path

from generator import Generator
from generators.header.header_generator import HeaderGenerator
from generators.typescript.typescript_generator import TypeScriptGenerator
from parse import parse
from preprocess import preprocess

GENERATOR_TYPE_HEADER = "header"
GENERATOR_TYPE_TYPESCRIPT = 'typescript'
GENERATOR_TYPES = [
    GENERATOR_TYPE_HEADER,
    GENERATOR_TYPE_TYPESCRIPT
]

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("krita_dir", help="Path to Krita repo")
    parser.add_argument("-o", "--output-dir", help="Path to output generated files", required=True)
    parser.add_argument("-g", "--generator-type", help="Which generator to use", required=True, choices=GENERATOR_TYPES)
    args = parser.parse_args()
    krita_dir = Path(args.krita_dir)
    output_dir = Path(args.output_dir)
    generator_type = args.generator_type

    generator: Generator = None
    if generator_type == GENERATOR_TYPE_HEADER:
        generator = HeaderGenerator()
    elif generator_type == GENERATOR_TYPE_TYPESCRIPT:
        generator = TypeScriptGenerator()

    if generator is None:
        raise ValueError(f"""Could not find generator of type: {generator_type}""")

    print("Krita source directory: " + str(krita_dir))
    print("Output directory: " + str(output_dir))
    file_paths = generator.get_file_paths(krita_dir)
    for file_path in file_paths:
        full_path = krita_dir / file_path
        source_code = full_path.read_text()
        # cxxheaderparser can't parse certain preprocessor directives so we preprocess first
        preprocessed = preprocess(source_code) 
        parsed_data = parse(preprocessed)
        generator.process(full_path, output_dir, parsed_data)

main()
