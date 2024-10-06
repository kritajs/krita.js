#!/usr/bin/env node
const fs = require('fs');
const { Readable } = require('stream');
const { finished } = require('stream/promises');
const sevenBin = require("7zip-bin");
const { extractFull } = require("node-7z");

const DEPS = {
  qt: () => {
    const BASE = "https://invent.kde.org"
    const PROJECT_ID = "dkazakov%2Fkrita-ci-artifacts-windows-qt5.15";
    // const PACKAGE_ID = 463825;
    const PACKAGE_NAME = "ext_qt";
    const PACKAGE_VERSION = "master-1723721868";
    return `${BASE}/api/v4/projects/${PROJECT_ID}/packages/generic/${PACKAGE_NAME}/${PACKAGE_VERSION}/archive.tar`;
  },
  ultralight: () => "https://ultralight-files.sfo3.cdn.digitaloceanspaces.com/ultralight-sdk-1.3.0-win-x64.7z",
};

const TEMP_PATH = "./temp";
const OUTPUT_PATH = "./deps";

async function install() {
  if (!fs.existsSync(TEMP_PATH)) {
    fs.mkdirSync(TEMP_PATH);
  }

  for (const [depName, getDownloadUrl] of Object.entries(DEPS)) {
    const outputPath = `${OUTPUT_PATH}/${depName}`;
    // Skip already downloaded dependencies
    if (fs.existsSync(outputPath)) continue;

    // Fetch and save archive
    const { body } = await fetch(getDownloadUrl());
    const downloadDestination = `${TEMP_PATH}/${depName}`;
    const stream = fs.createWriteStream(downloadDestination);
    await finished(Readable.fromWeb(body).pipe(stream));

    // Extract binaries to output folder
    const unzip = extractFull(downloadDestination, outputPath, {
      $bin: sevenBin.path7za,
    });
  }
}

install();
