# DSCS Mod Loader
A dynamic library intended to allow to extending, patching and modifying Digimon Story Cyber Sleuth Comeplete Edition on PC.

It is supposed to be injected using a [custom freetype.dll](https://github.com/SydMontague/DSCS-Freetype) and can by itself load additional [plugins](https://github.com/SydMontague/DSCSPluginTemplate/).

The project is not ready for production or supported in any shape or form. Whatever you do is on your own risk.

**This is an alpha, meaning that features are missing, not finished, and/or subject to breaking changes.**

# Installation
Download the [latest release](https://github.com/SydMontague/DSCSModLoader/releases) and extract the archive.

Copy both the `app_digister` and `resources` folder into the root folder of your game merging them with the already
existing folders of the same name.

You may want to adjust the `config.toml` to your desires using a regular text editor.

# Features
- Fixed movement speed being tied to FPS
- Fixed crash when using custom keyboard layouts
- Fixed black screen issue on Steam Deck/Gamescope
- Added Co-Save to allow extending the save format
- Added scan data co-save module, allowing to store scan data for more than 400 Digimon
- Added seen data co-save module, allowing to store seen data for more than 400 Digimon
- Added custom flag table co-save module
- Added custom workspace co-save module
- Added script functions
  - `void Debug.Log(String message)` - prints message in log
  - `int Digimon.GetScan(int digimonId)` - get scan progress for Digimon
  - `void Digimon.SetScan(int digimonId, int scan)` - set scan progress for Digimon
  - `void Digimon.AddScan(int digimonId, int scan)` - adds scan progress for Digimon
  - `bool ModLoader.GetFlag(string tableName, int flagId)` - gets state of flag in given table
  - `void ModLoader.SetFlag(string tableName, int flagId)` - sets flag in given table
  - `void ModLoader.ClearFlag(string tableName, int flagId)` - resets flag in given table
  - `void ModLoader.StorageSetInt(string tableName, int id, int value)` - sets workspace field to given int
  - `void ModLoader.StorageSetFloat(string tableName, int id, float value)` - sets workspace field to given float
  - `int ModLoader.StorageGetInt(string tableName, int id)` - gets workspace field as int
  - `float ModLoader.StorageGetFloat(string tableName, int id)` - gets workspace field as float
- Added functionality to disable VSync
- Added functionality to set the FPS limit
- Added functionality to speedup/slowdown the game
- Added ability to load binary patches during startup
- Added plugin loader, see [Plugin Template](https://github.com/SydMontague/DSCSPluginTemplate/)

# Patches
In order to apply custom patches you have to create a plain .txt file in the `resource/patches` folder.
A patch is formatted like `offset=<list of hex bytes>`. Lines that start with `;` are comments.

Example:
```
; patch "Acceleration" hacking skill for Cyber Sleuth
0x227AD6=63

; DInput background
0x57EC91=0A
; disable WindowsHasFocus chcek
0x57DBDA=90 90
0x57DCD7=EB

; disable isNPC check for hacking skills
0x229B87=90 90
; disable modelName.startsWith("pc") check for costume menu
0x366413=90 90 90 90 90 90
```

# Plugins
To install plugins, place the `dll` file into the `resources/plugins` folder. 
Please keep in mind that plugins can run arbitrary code and thus have the potential to be malicious. Only use plugins you trust!

There is no proper documentation for writing plugins as of yet. A template can be found here:
https://github.com/SydMontague/DSCSPluginTemplate/

Please note: this project is licensed under GPLv3, which requires your plugins to be distributed under a compatible license.
