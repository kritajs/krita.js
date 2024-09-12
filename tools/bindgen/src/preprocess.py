import io
import re
import typing
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
            raise Exception("\n".join(self.errors))
        elif self.return_code:
            raise Exception("Failed with exit code %d" % self.return_code)
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

def preprocess(file_name: str, source_code: str) -> str:
    pp = CustomPreprocessor()
    preprocessed = pp.parse(source_code, file_name)
    return preprocessed
