import { installArchive } from "../utils/install_archive";

const BASE_URL = "https://ultralight-files.sfo3.cdn.digitaloceanspaces.com";
const FILENAME = "ultralight-sdk-1.3.0-win-x64.7z";

export function installUltralight(outputPath: string) {
  return installArchive(`${BASE_URL}/${FILENAME}`, FILENAME, outputPath);
}
