import argparse
import dataclasses
import glob
import io
import json
import re
import typing
from cxxheaderparser.simple import parse_string
from pathlib import Path
from pcpp import Preprocessor, OutputDirective, Action

# Custom preprocessor for pcpp
class CustomPreprocessor(Preprocessor):
    def __init__(
        self,
        encoding: typing.Optional[str] = None,
        passthru_includes: typing.Optional["re.Pattern"] = None,
    ):
        Preprocessor.__init__(self)
        self.errors: typing.List[str] = []
        self.assume_encoding = encoding
        self.passthru_includes = passthru_includes
        # Remove the `#line` directives that pcpp inserts
        self.line_directive = None

    def parse(self, input, source=None):
        super().parse(input, source)
        if self.errors:
            raise Exception("\n".join(pp.errors))
        elif self.return_code:
            raise Exception("Failed with exit code %d" % pp.return_code)
        fp = io.StringIO()
        self.write(fp)
        fp.seek(0)
        output = fp.read()

        # Remove various defines and macros
        patterns = [
            # Auto-generated export define
            r"KRITALIBKIS_EXPORT",
            # Qt macros
            r"Q_OBJECT",
            r"Q_SLOTS",
            r"Q_SIGNALS:",
            r"Q_DECLARE_METATYPE.+",
            r"Q_DISABLE_COPY.+",
            r"Q_PROPERTY.+"
        ]
        combined_pattern = r'|'.join(map(r'(?:{})'.format, patterns))
        output = re.sub(combined_pattern, '', output)
        return output

    def on_error(self, file, line, msg):
        self.errors.append(f"{file}:{line} error: {msg}")

    # Ignore errors when an include is not found
    def on_include_not_found(self, *ignored):
        raise OutputDirective(Action.IgnoreAndPassThrough)
    
    # Pass through comments
    def on_comment(self, tok):
        return True

# Script requires caller to pass in path to the `libkis` directory
parser = argparse.ArgumentParser()
parser.add_argument("libkis_dir")
args = parser.parse_args()
libkis_dir = Path(args.libkis_dir)
print("libkis directory: " + str(libkis_dir))

# Parse all libkis headers
files = glob.glob("*.h", root_dir=str(libkis_dir))
for file_name in files:
    print("Processing " + file_name + "...")
    file_path = libkis_dir / file_name

    # Preprocess to remove directives, macros etc.
    pp = CustomPreprocessor()
    preprocessed = pp.parse(file_path.read_text(), file_name)
    # Write preprocessed header to a file
    preprocessed_file = Path(__file__).parent / "generated" / file_name
    preprocessed_file.parent.mkdir(exist_ok=True, parents=True)
    preprocessed_file.write_text(preprocessed)

    # Parse preprocessed output
    parsed_data = parse_string(preprocessed)
    ddata = dataclasses.asdict(parsed_data)
    json_data = json.dumps(ddata, indent=4)
    # Write parser output to a JSON file
    parsed_file = Path(__file__).parent / "generated" / (file_name + ".json")
    parsed_file.parent.mkdir(exist_ok=True, parents=True)
    parsed_file.write_text(json_data)
