# Tibia IP-Changer

A cross-platform IP-Changer for Open Tibia written in C++. Tested for versions up to 10.98.<br />
Prebuilt binaries under bin/

### Build Instructions for Premake

#### Windows
1. Edit paths.lua to set the directory of your Qt-installation.<br />
2. Run Generate.bat

#### Linux/macOS
Edit paths.lua to set the lib/bin/include directories for your Qt-installation, then
```
premake5 gmake
```

### Required libraries
Qt5Widgets<br />
Qt5Gui<br />
Qt5Core

```
sudo apt-get install qtbase5-dev
```
