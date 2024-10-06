import fs from "fs";
import { installKrita } from "./installers/krita";
import { installQt } from "./installers/qt";
import { installUltralight } from "./installers/ultralight";

export type InstallFn = (outputPath: string) => Promise<void>;

const DEPENDENCY_INSTALLERS = {
  krita: installKrita,
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
