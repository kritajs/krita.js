from cxxheaderparser.simple import ClassScope

INDENT = "    "
class TS_Param:
    def __init__(self, name: str, type: str):
        self.name = name
        self.type = type

    def __str__(self):
        return f"{self.name}: {self.type}"

class TS_Method:
    def __init__(self, name: str, params: list[TS_Param], return_type: str):
        self.name = name
        self.params = params
        self.return_type = return_type
    
    def __str__(self):
        params = []
        for param in self.params:
            params.append(str(param))
        return f"{self.name}({', '.join(params)}): {self.return_type};"

class TS_Ready_Class:
    def __init__(self, name: str, methods: list[TS_Method], file_name : str):
        self.name = name
        self.methods = methods
        self.file_name = file_name
    
    def __str__(self):
        methods = []
        for method in self.methods:
            methods.append(str(method))
        methods_joined_string = "\n\t".join(methods)
        return """declare class {0} {{
    {1}
}}""".format(self.name, methods_joined_string)

def remove_cpp_unique_syntax(phrase: str) -> str:
    return phrase.replace("*", "").replace("&", "").replace("const", "").strip()

def get_plain_class_name(c: ClassScope) -> str:
    # Construct class declaration
    class_decl = c.class_decl.typename.format()
    return class_decl.split(" ")[1]

### file_name is where the imported type will be imported from
def create_ts_ready_class(c: ClassScope, file_name: str) -> TS_Ready_Class:
    class_decl = get_plain_class_name(c) # class name
    methods: list[TS_Method] = [] 
    # each method should have (1) name; (2) params; (3) return type
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
                formatted_parameter = remove_cpp_unique_syntax(parameter.format()).split(" ")
                method_params.append(TS_Param(formatted_parameter[1],formatted_parameter[0]))
        method_return_type = remove_cpp_unique_syntax(method.return_type.format())
        methods.append(TS_Method(method_name, method_params, method_return_type))
    return TS_Ready_Class(class_decl, methods, file_name)
