import { TreeCursor } from "@lezer/common";
import { convertPrimitive, convertTemplate, convertTypeIdentifier } from "./type";

export class Method {
  return?: string;
  static: boolean = false;
  name: string = "";

  constructor(input: string, c: TreeCursor) {
    if (c.name === "static") {
      this.static = true;
      c.next();
    }

    // Determine return type
    switch (c.name) {
      case "PrimitiveType":
        this.return = convertPrimitive(input.substring(c.from, c.to));
        c.next();
        break;

      case "TypeIdentifier":
        this.return = convertTypeIdentifier(input.substring(c.from, c.to));
        c.next();
        break;
      
      case "TemplateType":
        this.return = convertTemplate(input, c.node.firstChild?.cursor()!);
        // Move to the next sibling node. Child nodes will be traversed by convertTemplate().
        c.next(false); 
        break;
    
      default:
        throw new Error(`Could not determine return type for method located at ${c.from}. The node type is ${c.name}.`);
    }

    c.iterate((node) => {
      if (c.name === "FieldIdentifier") {
        this.name = input.substring(c.from, c.to);
      }
    })
  }

  toString(): string {
    let output = "";

    if (this.static) output += "static ";
    output += `${this.name}()`;
    if (this.return && this.return !== "void") output += `: ${this.return}`;
    output += " { }";

    return output;
  }
}
