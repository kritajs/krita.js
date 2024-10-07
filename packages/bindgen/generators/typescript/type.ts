import { TreeCursor } from "@lezer/common";

const MAPPING: Map<string, string> = new Map([
  ["void", "void"],
  ["bool", "boolean"],
  ["int", "number"],
  ["double", "number"],
  ["qreal", "number"],
  ["QString", "string"],
  ["QStringList", "string[]"],
  ["QMap", "Map"],
]);

export function convertPrimitive(type: string): string {
  const mapping = MAPPING.get(type);
  return mapping === undefined ? type : mapping;
}

export function convertTypeIdentifier(type: string): string {
  let output = type.replace("*", "");
  return output.charAt(0).toUpperCase() === "Q" ? convertPrimitive(output) : output;
}

export function convertTemplate(input: string, c: TreeCursor): string {
  let baseType = input.substring(c.from, c.to);
  c.next();

  // If template is a QList, convert to T[]
  if (baseType === "QList") {
    let type = "";
    c.iterate(() => {
      // There should only be one of these nodes
      if (c.name === "TypeIdentifier") {
        type = convertTypeIdentifier(input.substring(c.from, c.to));
        return false;
      }
    });
    return `${type}[]`;
  }

  // Convert all other templates to T<arg1, arg2, ...>
  const args: string[] = [];
  c.iterate(() => {
    if (c.name === "TypeIdentifier") {
      args.push(convertTypeIdentifier(input.substring(c.from, c.to)));
    }
  });

  return `${convertTypeIdentifier(baseType)}<${args.join(", ")}>`;
}
