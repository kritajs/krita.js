import { TreeCursor } from "@lezer/common";
import { convertPrimitive, convertTemplate, convertTypeIdentifier } from "./type";

function getType(input: string, c: TreeCursor): string {
  // Determine return type
  switch (c.name) {
    case "PrimitiveType":
      return convertPrimitive(input.substring(c.from, c.to));

    case "TypeIdentifier":
      return convertTypeIdentifier(input.substring(c.from, c.to));
    
    case "TemplateType":
      return convertTemplate(input, c.node.firstChild?.cursor()!);
  
    default:
      throw new Error(`Could not determine TypeScript type for type located at ${c.from}. The node type is ${c.name}.`);
  }
}

class Parameter {
  type: string;
  name: string = "s";

  constructor(input: string, c: TreeCursor) {
    // Ignore any const keywords
    if (c.name === "const") {
      c.nextSibling();
    }

    this.type = getType(input, c);
    c.nextSibling();
    c.iterate(() => {
      if (c.name === "Identifier") {
        this.name = input.substring(c.from, c.to);
      }
    });
  }

  toString() {
    return `${this.name}: ${this.type}`;
  }
}

export class Method {
  static: boolean = false;
  return: string;
  parameters: Parameter[] = [];
  name: string = "";

  constructor(input: string, c: TreeCursor) {
    if (c.name === "static") {
      this.static = true;
      c.next();
    }

    // Determine return type
    this.return = getType(input, c);
    c.nextSibling();

    // Determine method name and parameters
    c.iterate(() => {
      if (c.name === "FieldIdentifier") {
        this.name = input.substring(c.from, c.to);
      } else if (c.name === "ParameterDeclaration") {
        this.parameters.push(new Parameter(input, c.node.firstChild?.cursor()!))
        return false;
      }
    })
  }

  toString(): string {
    let output = "";

    if (this.static) output += "static ";
    output += `${this.name}(${this.parameters.join(", ")})`;
    if (this.return !== "void") output += `: ${this.return}`;
    output += " { }";

    return output;
  }
}
