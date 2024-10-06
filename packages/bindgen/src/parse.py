import dataclasses
import io
from cxxheaderparser.simple import parse_string, ParsedData

# Converts ParsedData into a more readable format. This function removes fields that
# are equal to their default value so that we don't get an overly busy result.
# Copied verbatim from https://github.com/robotpy/cxxheaderparser/issues/4
def webrepr(data: ParsedData, defaults: bool = False, mxlen: int = 88) -> str:
    """
    A dumb black-like formatter for use on the cxxheaderparser webpage, which cannot
    use black.

    No guarantees are provided for this dumper. It probably generates valid python
    most of the time.
    """

    fp = io.StringIO()

    def _format(item, curlen: int, indent: str):
        # see if the default representation fits
        r = repr(item)
        if len(r) + curlen <= mxlen:
            fp.write(r)
            return

        # got to expand the item. Depends on what it is
        newindent = indent + "  "
        if isinstance(item, list):
            fp.write("[\n")
            curlen = len(newindent)
            for li in item:
                fp.write(newindent)
                _format(li, curlen, newindent)
                fp.write(",\n")
            fp.write(f"{indent}]")
        elif isinstance(item, dict):
            fp.write("{\n")
            curlen = len(newindent)
            for k, v in item.items():
                curlen = fp.write(f"{newindent}{k!r}:")
                _format(v, curlen, newindent)
                fp.write(",\n")
            fp.write(f"{indent}}}")
        elif dataclasses.is_dataclass(item):
            # always write the name, then process like a dict
            fp.write(f"{item.__class__.__qualname__}(\n")
            fields = dataclasses.fields(item)
            written = False
            for field in fields:
                # check to see if this is a default value, exclude those
                v = getattr(item, field.name)
                if not defaults and field.repr and field.compare:
                    if field.default_factory is not dataclasses.MISSING:
                        default = field.default_factory()
                    else:
                        default = field.default
                    if v == default:
                        continue
                curlen = fp.write(f"{newindent}{field.name}=")
                _format(v, curlen, newindent)
                fp.write(",\n")
                written = True
            if written:
                fp.write(indent)
            fp.write(")")
        else:
            # I give up, just write it. It's probably fine.
            fp.write(r)

    _format(data, 0, "")
    return fp.getvalue()

def parse(source_code: str) -> ParsedData:
    parsed_data = parse_string(source_code)
    return parsed_data
