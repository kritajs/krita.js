import fs from "fs";
import { installUltralight } from "./installers/ultralight";
import { installQt } from "./installers/qt";

export type InstallFn = (outputPath: string) => Promise<void>;

const DEPENDENCY_INSTALLERS = {
  qt: installQt,
  ultralight: installUltralight,
} as const satisfies Record<string, InstallFn>;

async function install() {
  for (const [depName, install] of Object.entries(DEPENDENCY_INSTALLERS)) {
    const outputPath = `./installed/${depName}`;
    // Skip already downloaded dependencies
    if (fs.existsSync(outputPath)) continue;
    await install(outputPath);
  }
}

install();
