import { createWriteStream, existsSync, mkdirSync } from "fs";
import { Readable } from "stream"
import { finished } from "stream/promises";
import { ReadableStream } from 'stream/web';
import { path7za } from "7zip-bin";
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
  if (!existsSync(TEMP_PATH)) {
    mkdirSync(TEMP_PATH);
  }

  // Fetch and save archive
  const res = await fetch(url);
  const body = res.body as ReadableStream<any>;
  if (!body) {
    throw new Error("Could not fetch dependency at URL: " + url);
  }

  const downloadDestination = `${TEMP_PATH}/${downloadName}`;
  const stream = createWriteStream(downloadDestination);
  await finished(Readable.fromWeb(body).pipe(stream));

  // Extract binaries to output folder
  extractFull(downloadDestination, outputPath, {
    $bin: path7za,
  });
}
