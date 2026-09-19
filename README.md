# Unlock Radio Stations - Native Edition (GTA V Enhanced)

Unlocks all GTA Online public radio stations in GTA V Enhanced Story Mode, including:

- Still Slipping Los Santos
- Kult FM
- The Music Locker
- MOTOMAMI Los Santos
- iFruit Radio
- Media Player
- Los Santos Underground Radio
- and every other public station

This is a native ASI plugin. It talks to Script Hook V directly, so it does not require ScriptHookVDotNet or .NET Framework.


## Why this exists

The existing way to unlock the Online stations on GTA V Enhanced is a ScriptHookVDotNet script (https://www.gta5-mods.com/scripts/unlock-radio-stations-enhanced-edition). That script needs ScriptHookVDotNet Enhanced and the .NET Framework, which is fragile on Linux under Proton/Wine, ScriptHookVDotNet crashes during startup (wine-mono throws an ObjectDisposedException in its TLS setup), so the .NET script cannot load.

This project reimplements the same idea as a native Script Hook V plugin (.asi). It has no .NET dependency, so it runs on Windows and on Linux through Proton/Wine with no extra runtime beyond Script Hook V.


## Requirements

- GTA V Enhanced (Story Mode).
- Script Hook V by Alexander Blade, in a version that supports your exact game build
  http://www.dev-c.com/gtav/scripthookv/
- Enhanced ASI loader xinput1_4.dll
https://github.com/ThirteenAG/Ultimate-ASI-Loader/releases

## Installation

1. Install Script Hook V in your game folder (the folder that contains `GTA5_Enhanced.exe`):
   - `ScriptHookV.dll`
   - `xinput1_4.dll` (ASI loader for the Enhanced build)

2. Copy `UnlockRadioStations.asi` into that same game folder.

3. Launch GTA V Enhanced in Story Mode. The radio stations are unlocked automatically on load and stay unlocked.


### Linux (Proton / Wine)

On Linux the Enhanced ASI loader is still `xinput1_4.dll`, but Wine/Proton ships its own built-in `xinput1_4` that will shadow the copy you put in the game folder. Force the local (native) one with this environment variable / launch argument:

```
WINEDLLOVERRIDES=xinput1_4=n,b
```

How you apply it depends on your setup, for example:

- Steam: game Properties -> Launch Options:

  ```
  WINEDLLOVERRIDES="xinput1_4=n,b" %command%
  ```

- Heroic / Lutris / Bottles / Faugus and other launchers: add it to the game's environment variables or launch options field.

- Plain Wine:

  ```
  WINEPREFIX=/path/to/prefix WINEDLLOVERRIDES=xinput1_4=n,b wine GTA5_Enhanced.exe
  ```

You only need to override `dinput8` if another mod uses it; this mod does not.


## Uninstall

Delete `UnlockRadioStations.asi` from the game folder.


## Important

Story Mode only. Do not use it in GTA Online. Script Hook V does not work online and BattlEye will flag injected DLLs.


## How it works

The plugin registers a script with Script Hook V and calls the game native:

```
AUDIO::LOCK_RADIO_STATION(stationName, FALSE)
```

for each public radio station (27 stations, by their internal stable audio names such as `RADIO_01_CLASS_ROCK`, `RADIO_37_MOTOMAMI`, etc.). Passing `FALSE` clears the "locked" state so the station appears in the Story Mode radio wheel. It re-applies the calls periodically so the stations stay unlocked after save loads and cutscenes.

It is a tiny x86-64 DLL whose only imports are `KERNEL32.dll` and `msvcrt.dll`, and it resolves the Script Hook V exports at runtime.


## Building from source

Source: `UnlockRadioStations.cpp`. A MinGW-w64 (x86-64) cross toolchain is enough:

```
x86_64-w64-mingw32-g++ -O2 -s -shared -static-libgcc -static-libstdc++ -o UnlockRadioStations.asi UnlockRadioStations.cpp
```


## Credits

- Original mod idea / Legacy "Unlock Radio Stations v1.1" by reneeter123 - [libertycity.net](https://libertycity.net/files/gta-5/174682-unlock-radio-stations-v1.1.html)
- Enhanced station list reference (internal names) - b3hold's ["Unlock Radio Stations - Enhanced Edition"](https://www.gta5-mods.com/scripts/unlock-radio-stations-enhanced-edition)
- Script Hook V by Alexander Blade - [dev-c.com](https://www.dev-c.com/gtav/scripthookv/)
- GTA V natives reference: alloc8or and the GTA modding community - [nativedb](https://github.com/alloc8or/gta5-nativedb-data)

This is an independent native reimplementation. It does not contain any files from the mods above; only the publicly known station names and the game's own radio native are used. Please keep the credits above if you republish.


## Disclaimer

Provided as-is, no warranty. Single-player use only. Use at your own risk.
