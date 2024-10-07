
import { parser } from "@lezer/cpp";
import { existsSync, mkdirSync, readFileSync, writeFileSync } from "fs";
import { toClassString } from "./class";
import { Method } from "./method";

const OUTPUT_PATH = "./dist";
const DEBUG = true;

function main() {
  const filename = "Krita.h"
  const input = readFileSync(`../deps/installed/krita/libs/libkis/${filename}`, "utf-8");
  const tree = parser.parse(input);
  const c = tree.cursor();

  let methods: Method[] = [];
  let isPublic = false; 

  // Traverse the syntax tree
  c.iterate((node) => {
    switch (node.name) {
      // In C++, access specifiers aren't part of the method declaration which means we
      // can't tell if a method is public just from the method declaration.
      // To solve this, we keep track of the most recent access specifier. If it's "public",
      // then we can assume that all methods that come after are public (until a non-public
      // access specifier is encountered).
      case "Access":
        const access = input.substring(c.from, c.to);
        isPublic = access === "public";
        break;
    
      case "FieldDeclaration":
        if (!isPublic) break;
        methods.push(new Method(input, node.node.firstChild?.cursor()!));
        return false;

      default:
        break;
    }
  });

  if (!existsSync(OUTPUT_PATH)) {
    mkdirSync(OUTPUT_PATH);
  }
  const name = filename.replace(".h", "");
  writeFileSync(`${OUTPUT_PATH}/${name}.ts`, toClassString(name, methods));

  if (DEBUG) {
    const DEBUG_INDENT = 4;
    let output = "";
    let level = 0;
    const debugCursor = tree.cursor();

    debugCursor.iterate(
      () => {
        output += " ".repeat(level * DEBUG_INDENT) + `${debugCursor.name} ${debugCursor.from}-${debugCursor.to}\n`;
        level += 1;
      },
      () => {
        level -= 1;
      },
    );

    writeFileSync(`${OUTPUT_PATH}/${name}.debug.txt`, output);
  }
}

main();
