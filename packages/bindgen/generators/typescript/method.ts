import { TreeCursor } from "@lezer/common";
import { Parameter } from "./parameter";
import { getType, Type, typeToString } from "./type";

export class Method {
  static: boolean = false;
  return?: Type;
  parameters: Parameter[] = [];
  name: string = "";

  constructor(input: string, c: TreeCursor) {
    if (c.name === "static") {
      this.static = true;
      c.nextSibling();
    } else if (c.name === "virtual") {
      c.nextSibling();
    }

    // Cursor should now be at the return type.
    const returnType = getType(input, c);
    if (returnType.name !== "void") {
      this.return = returnType;
    }
    c.nextSibling();

    // Parse method name and parameters
    c.iterate(() => {
      if (c.name === "FieldIdentifier") {
        this.name = input.substring(c.from, c.to);
      } else if (c.name === "ParameterDeclaration") {
        this.parameters.push(new Parameter(input, c.node.firstChild?.cursor()!))
        return false;
      }
    })
  }

  toString(body: string): string {
    let output = "";

    if (this.static) {
      output += "static ";
    }
  
    output += `${this.name}(${this.parameters.join(", ")})`;
  
    if (this.return && this.return.name !== "void") {
      output += `: ${typeToString(this.return)}`;
    }
  
    output += ` { ${body} }`;

    return output;
  }
}
