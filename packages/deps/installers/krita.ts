import { simpleGit } from "simple-git";

const URL = "https://invent.kde.org/graphics/krita.git";

export async function installKrita(outputPath: string) {
  const git = simpleGit();
  await git.clone(URL, outputPath, {
    "--depth": 1,
    "--branch": "v5.2.6",
  });
}
