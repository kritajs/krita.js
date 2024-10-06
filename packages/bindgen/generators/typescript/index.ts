
import { parser } from "@lezer/cpp";
import { existsSync, mkdirSync, readFileSync, writeFileSync } from "fs";
import { toClassString } from "./class";
import { Method } from "./method";

const OUTPUT_PATH = "./dist";
const DEBUG = true;
const DEBUG_INDENT = 4;

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
      // Keep track of the most recent "Access" node. We do this because we only want to
      // export public methods but access specifiers aren't part of the method declaration.
      case "Access":
        const access = input.substring(c.from, c.to);
        isPublic = access === "public";
        break;
    
      case "FieldDeclaration":
        if (!isPublic) break;
        methods.push(new Method(input, node.node.cursor()));
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
    let debug = "";
    let level = 0;
    const debugCursor = tree.cursor();

    debugCursor.iterate(
      () => {
        debug += " ".repeat(level * DEBUG_INDENT) + `${debugCursor.name} ${debugCursor.from}-${debugCursor.to}\n`;
        level += 1;
      },
      () => {
        level -= 1;
      },
    );

    writeFileSync("./debug.txt", debug);
  }
}

main();
