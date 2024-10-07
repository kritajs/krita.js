import { TreeCursor } from "@lezer/common";

const MAPPING: Record<string, string | undefined> = {
  "void": "void",
  "bool": "boolean",
  "qreal": "number",
  "QString": "string",
  "QStringList": "string[]",
} as const;

export function convertPrimitive(type: string): string {
  const mapping = MAPPING[type];
  return mapping === undefined ? type : mapping;
}

export function convertTypeIdentifier(type: string): string {
  let output = type.replace("*", "");
  return output.charAt(0) === "Q" ? convertPrimitive(output) : output;
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

  // Other template types will convert to T<arg1, arg2, ...>
  const args: string[] = [];
  c.iterate(() => {
    if (c.name === "TypeIdentifier") {
      args.push(convertTypeIdentifier(input.substring(c.from, c.to)));
    }
  });

  return `${convertTypeIdentifier(baseType)}<${args.join(", ")}>`;
}
