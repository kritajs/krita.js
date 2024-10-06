import { installArchive } from "../utils/install_archive";

const BASE = "https://invent.kde.org"
const PROJECT_ID = "dkazakov%2Fkrita-ci-artifacts-windows-qt5.15";
// const PACKAGE_ID = 463825;
const PACKAGE_NAME = "ext_qt";
const PACKAGE_VERSION = "master-1723721868";
const URL = `${BASE}/api/v4/projects/${PROJECT_ID}/packages/generic/${PACKAGE_NAME}/${PACKAGE_VERSION}/archive.tar`;

export function installQt(outputPath: string) {
  return installArchive(URL, "qt.tar", outputPath);
}
