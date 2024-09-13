from cxxheaderparser.simple import ClassScope

INDENT = "    "

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

        return_type = method.return_type.format()
        name = method.name.format()
        params = ", ".join(map(lambda param: param.format(), method.parameters))
        method_decl = INDENT
        if method.static:
            method_decl += "static "
        method_decl += f"{return_type} {name}({params})"
        if method.const:
            method_decl += " const"
        method_decl += ";"

        if method.access == "public":
            public_methods.append(method_decl)
        elif method.access == 'protected':
            protected_methods.append(method_decl)

    public = "\n".join(public_methods)
    protected = "\n".join(protected_methods)

    output = f"""{class_decl}
{{
public:
{public}

{"protected:" if len(protected_methods) else ""}
{protected}
}};
"""

    return (output, base_class_includes)