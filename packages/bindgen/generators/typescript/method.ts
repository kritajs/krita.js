import { TreeCursor } from "@lezer/common";

export class Method {
  return?: string;
  static: boolean = false;
  name: string = "";

  constructor(input: string, c: TreeCursor) {
    let stopTraversal = false;

    c.iterate((node) => {
      switch (node.name) {
        case "static":
          this.static = true;
          break;

        case "FieldIdentifier":
          this.name = input.substring(c.from, c.to);
          break;

        case "FieldDeclaration":
          // `iterate()` also traverses sibling nodes which means
          // we'll also traverse other methods. We do this to
          // automatically stop traversal when we encounter the
          // next method.
          if (stopTraversal) return false;
          stopTraversal = true;
          break;

        default:
          break;
      }
    });
  }

  toString(): string {
    let output = "";

    if (this.static) output += "static ";
    output += `${this.name}()`;
    if (this.return) output += `: ${this.return}`;
    output += " { }";

    return output;
  }
}
