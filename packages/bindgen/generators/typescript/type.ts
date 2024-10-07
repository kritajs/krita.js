import { TreeCursor } from "@lezer/common";

interface PrimitiveType {
  type: "primitive",
  name: string,
}

interface IdentifierType {
  type: "identifier",
  name: string,
}
interface ArrayType {
  type: "array",
  name: string,
}

interface TemplateType {
  type: "template",
  // First value is the class name. Subsequent values are the template arguments.
  name: string[],
}

export type Type = PrimitiveType | IdentifierType | ArrayType | TemplateType;

const MAPPING: Map<string, string> = new Map([
  ["void", "void"],
  ["bool", "boolean"],
  ["int", "number"],
  ["float", "number"],
  ["double", "number"],
  ["qreal", "number"],
  ["QString", "string"],
  ["QStringList", "string[]"],
  ["QMap", "Map"],
]);

const VALID_TS_TYPES = new Set(MAPPING.values());

export function isValidTsType(type: string): boolean {
  return VALID_TS_TYPES.has(type);
}

function parsePrimitive(type: string): PrimitiveType | undefined {
  const mapping = MAPPING.get(type);
  return mapping ? { type: "primitive", name: mapping } : undefined;
}

function parseTypeIdentifier(type: string): PrimitiveType | IdentifierType {
  const output = type.replace("*", "");
  // Try to convert to primitive. If it failed, just return it as an identifier.
  return parsePrimitive(output) ?? { type: "identifier", name: output };
}

function parseTemplate(input: string, c: TreeCursor): ArrayType | TemplateType {
  let templateName = input.substring(c.from, c.to);
  c.next();

  // If template is a QList, convert to T[]
  if (templateName === "QList") {
    let typeName = "";
    c.iterate(() => {
      // There should only be one of these nodes
      if (c.name === "TypeIdentifier") {
        typeName = parseTypeIdentifier(input.substring(c.from, c.to)).name;
        return false;
      }
    });
    return { type: "array", name: typeName };
  }

  // All other templates should be T<arg1, arg2, ...>
  const args: string[] = [];
  c.iterate(() => {
    if (c.name === "TypeIdentifier") {
      args.push(parseTypeIdentifier(input.substring(c.from, c.to)).name);
    }
  });

  return { type: "template", name: [parseTypeIdentifier(templateName).name, ...args]}
}

export function getType(input: string, c: TreeCursor): Type {
  // Determine return type
  switch (c.name) {
    case "PrimitiveType":
      return parsePrimitive(input.substring(c.from, c.to))!;

    case "TypeIdentifier":
      return parseTypeIdentifier(input.substring(c.from, c.to));
    
    case "TemplateType":
      return parseTemplate(input, c.node.firstChild?.cursor()!);
  
    default:
      throw new Error(`Could not determine TypeScript type for type located at ${c.from}. The node type is ${c.name}.`);
  }
}

export function typeToString(type: Type): string {
    if (type.type === "template") {
      return type.name[0] + `<${type.name.slice(1).join(", ")}>`
    } else if (type.type === "array") {
      return type.name + "[]";
    }
    return type.name;
}
