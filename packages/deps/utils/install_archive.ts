import fs from "fs";
import { Readable } from "stream"
import { finished } from "stream/promises";
import { ReadableStream } from 'stream/web';
import sevenBin from "7zip-bin";
import { extractFull } from "node-7z";

const TEMP_PATH = "./temp";

export async function installArchive(
  /**
   * URL to download from.
   */
  url: string,
  /**
   * Filename to save the download as in the `temp` folder.
   */
  downloadName: string,
  /**
   * Where to extract the archive.
   */
  outputPath: string
) {
  if (!fs.existsSync(TEMP_PATH)) {
    fs.mkdirSync(TEMP_PATH);
  }

  // Fetch and save archive
  const res = await fetch(url);
  const body = res.body as ReadableStream<any>;
  if (!body) {
    throw new Error("Could not fetch dependency at URL: " + url);
  }

  const downloadDestination = `${TEMP_PATH}/${downloadName}`;
  const stream = fs.createWriteStream(downloadDestination);
  await finished(Readable.fromWeb(body).pipe(stream));

  // Extract binaries to output folder
  extractFull(downloadDestination, outputPath, {
    $bin: sevenBin.path7za,
  });
}
