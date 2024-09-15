from cxxheaderparser.simple import ClassScope, ParsedData
from cxxheaderparser.types import EnumDecl

INDENT = "    "

# ==================================================================================

def Template(
    includes: str,
    forward_decls: str,
    classes: str,
) -> str:
    content: list[str] = []
    if includes != "":
        content.append(includes)
    if forward_decls != "":
        content.append(forward_decls)
    if classes != "":
        content.append(classes)
    return f"""// This file is automatically generated by bindgen. DO NOT EDIT.

// Some libkis classes depend on QtGlobal typedefs so we just include it in every header for convenience
#include <QtGlobal>
// Contains common Qt includes and libkis forward declarations
#include "libkis.h"
{includes}

// Instead of determining which forward declarations we need on a per-file basis, just forward declare
// everything we might need.
class ColorizeMask;
class DockPosition;
class FilterMask;
class KisImageSP;
class KisNodeSP;
class ManagedColor;
class Palette;
class QDockWidget;
class QMainWindow;
class SelectionMask;
class Swatch;
class TransformMask;
class TransparencyMask;
{forward_decls}

{classes}"""

# ==================================================================================

def Enum(enum: EnumDecl) -> str:
    values: list[str] = []
    for value in enum.values:
        values.append(INDENT + value.name)
    values = f",\n{INDENT}".join(values)

    return f"""{INDENT}{enum.typename.format()}{{
    {values}
    }};"""

# ==================================================================================

def Class(c: ClassScope) -> tuple[str, set[str]]:
    base_class_includes: set[str] = set()

    # Extract base classes
    bases: list[str] = []
    for base in c.class_decl.bases:
        base_name = base.typename.format()
        # If it's a base class, then we MUST add its #include. Base classes cannot be forward declared.
        base_class_includes.add(base_name)
        bases.append(base.access + " " + base_name)

    # Construct class declaration
    class_decl = c.class_decl.typename.format()
    if (len(bases)):
        class_decl += f" : {', '.join(bases)}"

    # Extract methods. Skip private methods.
    public_methods: list[str] = []
    protected_methods: list[str] = []
    for method in c.methods:
        # Skip constructors, destructors and operator overloads
        if method.constructor or method.destructor or method.operator is not None:
            continue

        name = method.name.format()
        params = ", ".join(map(lambda param: param.format(), method.parameters))
        method_decl = INDENT
        segments: list[str] = []
        if method.static:
            segments.append("static")
        if method.virtual:
            segments.append("virtual")
        if method.return_type:
            segments.append(f"{method.return_type.format()}")
        segments.append(f"{name}({params})")
        if method.const:
            segments.append("const")
        if method.override:
            segments.append("override")
        method_decl += " ".join(segments) + ";"

        if method.access == "public":
            public_methods.append(method_decl)
        elif method.access == 'protected':
            protected_methods.append(method_decl)
    public = "\n".join(public_methods)
    protected = "\n".join(protected_methods)

    # Extract enums. Assumes all enums are public.
    enums: list[str] = []
    for enum in c.enums:
        enums.append(Enum(enum))
    enums = "\n".join(enums)

    output = f"""{class_decl}
{{
public:
{enums}
{public}

{"protected:" if len(protected_methods) else ""}
{protected}
}};
"""

    return (output, base_class_includes)


# ==================================================================================

def generate_header(data: ParsedData) -> str:
    # Keep any existing forward declarations
    forward_decls: set[str] = set()
    for forward_decl in data.namespace.forward_decls:
        forward_decls.add(forward_decl.typename.format())

    # Parse classes
    classes: list[str] = []
    base_class_includes: set[str] = set()
    for c in data.namespace.classes:
        (output, _base_class_includes) = Class(c)
        base_class_includes = _base_class_includes
        classes.append(output)

    # Format includes. Note that we only add includes for base classes.
    # Everything else can be forward declared.
    includes: set[str] = set()
    for include in base_class_includes:
        # Use angle brackets for Qt includes
        if include.startswith("Q"):
            includes.add(f"#include <{include}>")
        else:
            includes.add(f"#include \"{include}.h\"")

    return Template(
        "\n".join(includes),
        "\n".join(map(lambda name: f"{name};", forward_decls)),
        "\n".join(classes)
    )