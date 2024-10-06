#!/usr/bin/env node
const fs = require('fs');
const { Readable } = require('stream');
const { finished } = require('stream/promises');
const sevenBin = require("7zip-bin");
const { extract } = require("node-7z");

const BASE_URL = "https://ultralight-files.sfo3.cdn.digitaloceanspaces.com";
const FILE = "ultralight-sdk-1.3.0-win-x64.7z";
const TEMP_PATH = "./temp";
const DOWNLOAD_PATH = `${TEMP_PATH}/${FILE}`;

async function install() {
  if (!fs.existsSync(TEMP_PATH)) fs.mkdirSync(TEMP_PATH);

  // Fetch and save archive
  const { body } = await fetch(`${BASE_URL}/${FILE}`);
  const stream = fs.createWriteStream(DOWNLOAD_PATH);
  await finished(Readable.fromWeb(body).pipe(stream));

  // Extract Ultralight binaries to bin folder
  const unzip = extract(DOWNLOAD_PATH, './bin', {
    recursive: true,
    $cherryPick: '*.dll',
    $bin: sevenBin.path7za,
  });

  function onEnd() {
    fs.rmSync(DOWNLOAD_PATH, {
      force: true,
    });
  }

  // Delete archive after extraction or error
  unzip.on('end', onEnd);
  unzip.on('error', (err) => {
    console.error(err);
    onEnd();
  });
}

install();
