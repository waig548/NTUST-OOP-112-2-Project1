# 112-2 Project 1

A simple(?) plumber game using C++.

## Member
waig548

## Content
| Name | Description |
| --- | --- |
| [libplumber](./libplumber) | The base library of the game |
| [plumber-cli](./plumber-cli) | A CLI implementation of the game, the requirement for this project |
| [plumber-gui](./plumber-gui) | A GUI implementation of the game using OpenGL and ImGui |

## Dependencies
- CMake

## Quickstart

```
git clone --recursive https://github.com/waig548/NTUST-OOP-112-2-Project1.git
cd NTUST-OOP-112-2-Project1
cmake -S . -B build
cmake --build build -j 8
```
Note: For MSVC toolchains, the minimum version is VS2022.\
You may also need to run these commands in the `Developer Command Prompt` to build.\
The executables will be in `build/out`.

Or simply clone this repo in Visual Studio, it should recognize the CMake scripts.

## Contribution

### waig548
- libplumber
    - library structures
    - base game logics
    - board generation
    - board file parser
- plumber-cli
    - terminal-based user interface
- plumber-gui
    - graphical user interface