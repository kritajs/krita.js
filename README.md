# krita.js

krita.js is a Krita plugin that allows users to write scripts using JavaScript.

## To do
- [x]  Don’t run Sciter as separate window DONE
- [ ]  Slot Sciter window into Qt widgets
- [ ]  Create JS bindings for Krita API
- [ ]  Generate .d.ts files for Krita API
- [ ]  Figure out interface for krita.js plugins
- [ ]  Set up packaging/release pipeline for multiple platforms

## Installation

First, download the [Sciter SDK](https://sciter.com/download/) and unzip it.

Next, download this repo and then create a `kritajs/bin` folder. Copy `sciter-js-sdk-main/bin/windows/x64/sciter.dll` into it. Your folder structure should look like:

```
krita.js/
├─ kritajs/
│  ├─ bin/
│  │  ├─ sciter.dll
│  ├─ lib/
│  ├─ kritajs.py
│  ├─ other files...
├─ kritajs.desktop
```

Now, you need to install the plugin into Krita. To do this, follow the **Manually** section of the [Managing Python plugins](https://docs.krita.org/en/user_manual/python_scripting/install_custom_python_plugin.html#manually) guide. Don't forget to enable the plugin after installation.

> If you are developing this plugin, it's better to install krita.js by using symlinks. See the **Developing krita.js** section for more info.

Finally, run krita.js by selecting **Tools > Scripts > Run krita.js**.

## Developing krita.js

When developing krita.js, it's useful to create symlinks from the Krita plugin folder to the cloned repo.

To do this on Windows, clone this repo and then run the commands below in a command prompt (not PowerShell). You may need to run the command prompt as an administrator.

Create symlink to `kritajs` folder:

```
mklink /D "C:\Users\myuser\AppData\Roaming\krita\pykrita\kritajs" "absolute\path\to\krita.js\kritajs"
```

Create symlink to `kritajs.desktop` file:

```
mklink /H "C:\Users\myuser\AppData\Roaming\krita\pykrita\kritajs.desktop" "absolute\path\to\krita.js\kritajs.desktop"
```
