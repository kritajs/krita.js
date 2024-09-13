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

def getPlainClassName(c: ClassScope) -> str:

    # Construct class declaration
    class_decl = c.class_decl.typename.format()
    for method in c.methods:
        # remove constructors
        if (method.constructor): continue
        # remove private access method - not sure if it should just be a string check against 'private' instead
        if (method.access != "public"): continue

        if (class_decl.split(" ")[1] != "View"): continue
        
        #ignore ones that start with ~ or operator or has no name
        if method.name.format() == [] or method.name.format().startswith("~") or method.name.format().startswith("operator"): 
            continue

        print(method.name.format())        

        if method.parameters == []:
            print("no parameters needed")
        else:
            for parameter in method.parameters:
                # remove pointer and reference signs FOR NOW -> NEED TO REVIEW THIS LATER
                print(parameter.format().replace("*", "").replace("&", ""))

        if (method.return_type == None): continue
        print(method.return_type.format().replace("*", "").replace("&", ""))
        print("\n")
    return class_decl.split(" ")[1]

def get_ts_ready_class(c: ClassScope) -> object:
    class_decl = c.class_decl.typename.format() # class name
    methods: list[TS_Method] = [] # each method should have (1) name; (2) params; (3) return type
    for method in c.methods:
        # ignore constructors
        if (method.constructor): continue
        # ignore private access method
        if (method.access == "private"): continue
        # ignore methods with names that start with ~ or operator or has no name
        if (method.name.format() == [] or 
            method.name.format().startswith("~") or 
            method.name.format().startswith("operator")): 
            continue
        method_name = method.name.format()
        method_params = []
        if method.parameters != []:
            for parameter in method.parameters:
                # remove pointer and reference signs and reformat to TS standard
                formatted_parameter = parameter.format().replace("*", "").replace("&", "").split(" ")
                method_params.append(formatted_parameter[1] + ': ' + formatted_parameter[0])
        method_return_type = method.return_type.format().replace("*", "").replace("&", "")
        # print("\n")
        methods.append(TS_Method(method_name, method_params, method_return_type))
        return TS_Ready_Class(class_decl, methods)

class TS_Method:
    def __init__(self, name: str, params: list, return_type: str):
        self.name = name
        self.params = params
        self.return_type = return_type

class TS_Ready_Class:
    def __init__(self, name: str, methods: list[TS_Method]):
        self.name = name
        self.methods = methods
