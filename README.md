# Power Supply Management
[![Build Status](https://dev.azure.com/vitark/PS-Management/_apis/build/status/vitark.PS-Management?branchName=main)](https://dev.azure.com/vitark/PS-Management/_build/latest?definitionId=1&branchName=main)
[![StandWithUkraine](https://raw.githubusercontent.com/vshymanskyy/StandWithUkraine/main/badges/StandWithUkraine.svg)](https://github.com/vshymanskyy/StandWithUkraine/blob/main/docs/README.md)

* [Power Supply Management](#power-supply-management)
   * [Features](#features)
   * [Getting PS-Management](#getting-ps-management)
   * [Compiling from source](#compiling-from-source)
      * [Dependencies](#dependencies)
      * [Compilation](#compilation)
         * [macOS and Linux](#macos-and-linux)
         * [Windows](#windows)
      * [Supported Hardware](#supported-hardware)
         * [Officially supported](#officially-supported)
         * [Not supported, but support can be easily added](#not-supported-but-support-can-be-easily-added)
   * [USB/COM cable and communications issues](#usbcom-cable-and-communications-issues)
      * [USB Cable](#usb-cable)
      * [COM with adaptors](#com-with-adaptors)
   * [Screenshots](#screenshots)
      * [PS-Management running on Windows 10](#ps-management-running-on-windows-10)
      * [PS-Management running on Ubuntu 22.04.1 LTS](#ps-management-running-on-ubuntu-22041-lts)
   * [Bugs, feature requests, ideas](#bugs-feature-requests-ideas)
   * [License](#license)
   * [Stand With Ukraine](#stand-with-ukraine)

Power Supply Management (PS-Management) is a GUI Qt Framework-based application for controlling laboratory power supplies of UNI-T UTP330xC and other devices that use the same SCPI.

<img width="895" alt="macos_screen" src="https://user-images.githubusercontent.com/7736508/213449855-d5db9de5-ab18-4824-9683-a3351c993ea2.png">

*PS-Management running on macOS 12.6*

## Features

* Cross-platform (macOS, Windows, Linux) written in Qt
* Full support of the UNI-T UTP330xC
* Fast UI and reaction with a device, adaptive pooling algorithm
* Ease for adding new capable or similar devices

## Getting PS-Management

You can download macOS, Windows (x86 only), and Linux versions from the GitHub [releases tab](https://github.com/vitark/PS-Management/releases) for this project.

## Compiling from source

### Dependencies

The following dependencies are required to run this software:
* CMake >= 3.4
* One of these Compilers:
    * GCC >= 4.9
    * clang >= 3.4
    * MSVC >= 16 (Visual Studio 2019)
    * MinGW >= 4.9
* Qt >= 5.15 (Qt5Widgets, Qt5Gui, Qt5Core, Qt5Svg, Qt5SerialPort)

### Compilation

#### macOS and Linux

Compiling PS-Management on macOS and Linux using UNIX make files.

```shell
# download the source code and change it to the directory
# create a build directory
mkdir build
cd build
# run cmake to create makefiles. Use -DCMAKE_PREFIX_PATH if cmake doesn't find
# your installation of Qt5 or define ENV export e.g. CMAKE_PREFIX_PATH=/Users/Qt/5.15.2/clang_64/lib/cmake
cmake -DCMAKE_BUILD_TYPE=Release ../
# now compile the source code and create the application. You can speed up
make
```

#### Windows

You can compile the application on Windows using CMake's [Visual Studio Generator](https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html#visual-studio-generators).

```shell
# create a build directory inside PS-Management source directory
mkdir build
cd build
# Assuming you are using Visual Studio 16 2019 windows installation and Qt 5.15.2 installed to D:\\Qt
# Please change these options so they suit your build environment.

# x64 version
cmake -DQt5_DIR="d:\\Qt\\5.15.2\\msvc2019_64\\lib\\cmake\\Qt5"  ../
cmake --build . --target bundle --config Release

# x86 version 
cmake -DQt5_DIR="d:\\Qt\\5.15.2\\msvc2019_64\\lib\\cmake\\Qt5" -G "Visual Studio 16 2019" -A Win32  ../
cmake --build . --target bundle --config Release
```

### Supported Hardware

Currently, the application only supports UNI-T devices using the [SCPI Protocol](https://github.com/vitark/PS-Management/blob/main/docs/UTP3300C%20English%20manual.pdf). Otherwise, it seems UNI-T devices are rebranded or repacked of [Korad KA300xP](http://koradtechnology.com/) and based on [Korad SCPI Protocol](https://sigrok.org/wiki/Korad_KAxxxxP_series), so Korad devices should to work also or can be accessible to added. Pull Requests for supporting new devices are welcome.

#### Officially supported:
* UNI-T UTP3303C
* UNI-T UTP3305C

#### Not supported, but support can be easily added:
* Korad KA3003P; KA3005P; KA3010P
* Korad KA6002P; KA6003P; KA6005P
* Tenma 72-2535; 72-2540 (rebranded KA30XXP)
* Tenma 72-2545; 72-2550 (rebranded KA60XXP)
* Vellemann PS3005D (rebranded KA3005P)

## USB/COM cable and communications issues
While developing the application, I tried many different cables, USB, and COM (with adaptors)

### USB Cable
Two meters and a longer generic USB2 cable didn't work on my set (Macbook via own USB host). Otherwise, an active USB cable (5m) is working fine. So try to use a USB2 cable as short as possible or an active USB.

### COM (with adaptors)
USB-Serial adaptor (ST Lab, USB-Serial-4, based on PL2303) works fine.

## Screenshots
### *PS-Management running on Windows 10*

![windows-Screenshot](https://user-images.githubusercontent.com/7736508/213516574-c8aa1049-60a9-4d86-8f7c-f4b4eb8f1567.png)

### *PS-Management running on Ubuntu 22.04.1 LTS*

![ubuntu-Screenshot](https://user-images.githubusercontent.com/7736508/213523550-c3a96186-f0bd-4e73-a215-5138870a77bd.png)

## Bugs, feature requests, ideas

Please use the [github bugtracker](https://github.com/vitark/PS-Management/issues)
to submit bugs or feature requests

## License
```
                    GNU GENERAL PUBLIC LICENSE
                     Version 3, 29 June 2007

Copyright (C) 2007 Free Software Foundation, Inc. <http://fsf.org/>
Everyone is permitted to copy and distribute verbatim copies
of this license document, but changing it is not allowed.
 
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
```

## Stand With Ukraine

[![SWUbanner](https://raw.githubusercontent.com/vshymanskyy/StandWithUkraine/main/banner-direct.svg)](https://github.com/vshymanskyy/StandWithUkraine/blob/main/docs/README.md)

### Additional terms of use for users from Russia and Belarus

By using the code provided in these repositories you agree with the following:
* Russia has [illegally annexed Crimea in 2014](https://en.wikipedia.org/wiki/Annexation_of_Crimea_by_the_Russian_Federation) and [brought the war in Donbas](https://en.wikipedia.org/wiki/War_in_Donbas) followed by [full-scale invasion of Ukraine in 2022](https://en.wikipedia.org/wiki/2022_Russian_invasion_of_Ukraine).
* Russia has brought sorrow and devastations to millions of Ukrainians, killed hundreds of innocent people, damaged thousands of buildings, and forced several million people to flee.
* [Putin khuylo!](https://en.wikipedia.org/wiki/Putin_khuylo!)

Glory to Ukraine! 🇺🇦
