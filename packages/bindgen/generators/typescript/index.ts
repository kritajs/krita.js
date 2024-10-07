
import { parser } from "@lezer/cpp";
import {
  copyFileSync,
  existsSync,
  mkdirSync,
  readdirSync,
  readFileSync,
  writeFileSync
} from "fs";
import * as path from "path";
import * as prettier from "prettier";
import { toClassString } from "./class";
import { Method } from "./method";

const LIBKIS_PATH = "../deps/installed/krita/libs/libkis";
const OUTPUT_PATH = "./dist";
const DEBUG = false;

const BLOCK_LIST = new Set([
  // Not used by anything
  "LibKisUtils",
  // No classes in here. Just includes and forward declarations
  "libkis",
]);

async function main() {
  const filePaths = readdirSync(LIBKIS_PATH, { withFileTypes: true })
    .filter((f) => f.isFile() && path.parse(f.name).ext === ".h")
    .map((f) => path.parse(`${LIBKIS_PATH}/${f.name}`))
    .filter((f) => !BLOCK_LIST.has(f.name));
  
  for (const path of filePaths) {
    let input = readFileSync(`${path.dir}/${path.base}`, "utf-8");
    input = input.replace("Q_SIGNALS:", ""); // TODO: Handle signals correctly
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
        case "AccessSpecifier":
          const access = input.substring(c.from, c.to);
          isPublic = access === "public Q_SLOTS:";
          break;
      
        case "FieldDeclaration":
          if (!isPublic) return false;

          methods.push(new Method(input, node.node.firstChild?.cursor()!));
          return false;

        default:
          break;
      }
    });

    if (!existsSync(OUTPUT_PATH)) {
      mkdirSync(OUTPUT_PATH);
    }

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

      writeFileSync(`${OUTPUT_PATH}/${path.name}.debug.txt`, output);
    }

    let output = toClassString(path.name, methods);
    output = await prettier.format(output, { semi: true, parser: "typescript" });
    writeFileSync(`${OUTPUT_PATH}/${path.name}.ts`, output);
    copyFileSync(`${__dirname}/static/kritajs.ts`, `${OUTPUT_PATH}/kritajs.ts`);
  }
}

main();
