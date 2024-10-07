import { Method } from "./method";

export function toClassString(name: string, methods: Method[]): string {
  const imports: Set<string> = new Set();

  // for (const m of methods) {
  //   if (m.return) {
  //     imports.add(m.return.name);
  //   }

  //   for (const p of m.parameters) {
  //     const segments = Array.isArray(p.type) ? p.type : [p.type];
  //     for (const s of segments) {
  //       imports.add(p.type);
  //     }
  //   }
  // }

  const methodStrings = methods.map(m => {
    let body = "";

    if (m.return) {
      body += " return ";
    }

    body += `${m.static ? "invokeStatic" : "invoke"}`;
    const id = `${m.static ? "this.__className__" : "this.__id__"}`;

    // Parameters
    body += `(${id}, "${m.name}"`;
    if (m.parameters.length > 0) {
      body += `, [${m.parameters.map(p => p.name).join(", ")}]`;
    }
    body += ")";

    return m.toString(body);
  });

  return `// This file is auto-generated by bindgen. DO NOT EDIT.
import { Id, invoke, invokeStatic } from "./kritajs";

// ${Array.from(imports.values()).join(", ")}

export class ${name} {
  private static readonly __className__ = "${name}";

  private constructor(private readonly __id__: Id) {}

${methodStrings.join("\n\n")}
}
`;
}
