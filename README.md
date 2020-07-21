# amiko_linux
AmiKo/CoMed for Linux done with wxWidgets and C++, 64 bit.

Prerequisites:

- [CMake](https://cmake.org/)

- GTK 3

        $ sudo apt install libgtk-3-dev

- WebKit2

        $ sudo apt install libwebkit2gtk-4.0-dev

- [SQlite](https://www.sqlite.org/) is built-in into the application, so there is no dependency on system libraries.

- JSON [nlohmann](https://github.com/nlohmann/json)

		$ git submodule init
		$ git submodule update

## Build Script
1. Download and install latest wxWidgets from source using build script.
2. Build script also has to download all data files, see OSX version.
3. Build script has to build executables named `AmiKo` and `CoMed`.

## Setup
1. Run `build.sh`
2. Edit `steps.conf`
3. Edit `seed.conf`
4. Run `build.sh` again.

### Notes when building wxWidgets and SQLite
1. For Mac in `steps.conf`
```
STEP_CONFIGURE_WXWIDGETS=y
STEP_COMPILE_WXWIDGETS=y
```
2. For Mac in `seed.conf`
```
CONFIG_GENERATOR_MK=y
```
### Notes when building AmiKo/CoMed
1. For Mac in `steps.conf`
```
STEP_CONFIGURE_APP=y
STEP_COMPILE_APP=y
```
2. For Mac in seed.conf
```
CONFIG_GENERATOR_XC=y
```

## macOS Installer
1. Create a .pkg Installer for macOS that installs all the DB files in to `~/.AmiKo` or `~/.CoMed`
