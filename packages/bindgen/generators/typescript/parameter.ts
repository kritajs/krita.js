import { TreeCursor } from "@lezer/common";
import { getType, Type, typeToString } from "./type";

export class Parameter {
  type: Type;
  name: string = "";

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
    return `${this.name}: ${typeToString(this.type)}`;
  }
}
