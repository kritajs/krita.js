import { existsSync } from "fs";
import { installKrita } from "./installers/krita";
import { installQt } from "./installers/qt";
import { installUltralight } from "./installers/ultralight";

type InstallFn = (outputPath: string) => Promise<void>;

const INSTALLERS = {
  krita: installKrita,
  qt: installQt,
  ultralight: installUltralight,
} as const satisfies Record<string, InstallFn>;

async function install() {
  for (const [depName, install] of Object.entries(INSTALLERS)) {
    const outputPath = `./installed/${depName}`;

    // Skip already downloaded dependencies
    if (existsSync(outputPath)) continue;
  
    await install(outputPath);
  }
}

install();
