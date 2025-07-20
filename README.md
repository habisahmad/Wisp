# Wisp - C++ Text Editor

**Wisp** is a simple, fast, minimal text editor built using C++ and the [FLTK](https://www.fltk.org/) library.

<img width="1282" height="798" alt="image" src="https://github.com/user-attachments/assets/f248a61f-c56e-43d6-8998-1974fee794bf" />

## Features
- Basic text editing (Cut, Copy, Paste, etc.)
- File operations (open, save, save as)
- Find and replace with dialog interface
- C/C++ syntax highlighting
- Line numbers
- Cross Platfrom

##  Build Instructions

### Requirements

- C++17 compatible compiler (e.g. `g++`)
- [FLTK 1.4+](https://www.fltk.org/)

### Build (macOS/Linux)

```bash
g++ main.cpp editor_window.cpp syntax_highlighting.cpp $(fltk-config --cxxflags --ldflags) -o wisp
./wisp

