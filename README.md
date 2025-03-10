# CHIP-8 Emulator in C++

This project is a [Chip-8](https://en.wikipedia.org/wiki/CHIP-8) emulator/interpreter implemented in C++ using the SDL2 library. 
It is a work in progress, not all opcodes have been implemented yet, for now just enough to run the **IBM Logo** and **CHIP-8 Logo** test ROMs,
both can be found on [Timendus' chip8-test-suite](https://github.com/Timendus/chip8-test-suite). 

---

## Features

- **ROM loading from command line**: chip-8 programs, with the appropriate max. size, can be loaded from the command line.
- **Fetch-Decode-Execute:** as of march 7, 2025, 11/35 opcodes are implemented.
  <details>
    <summary>See list of supported opcodes</summary>

    - `00E0` **CLS**: Clears screen.
    - `00EE` **RET**: Return from subroutine.
    - `1NNN` **JP *addr***: Jump to address.
    - `2NNN` **CALL *addr***: Call a subroutine at NNN.
    - `3XNN` **SE Vx, byte**: Skip next instruction if Vx is equal to byte.
    - `4XNN` **SNE Vx, byte**: Skip next instruction if Vx is not equal to byte.
    - `5XY0` **SE Vx, Vy**: Skip next instruction if Vx is equal to Vy.
    - `6XNN` **LD Vx *byte***: Loads register Vx with NN.
    - `7XNN` **ADD Vx, *byte***: Add NN to register Vx.
    - `ANNN` **LD I**: Loads Index register with NNN.
    - `DXYN` **DRW Vx, Vy, N**: Draws a N height sprite located at the memory
       position pointed by I register, in position (Vx, Vy).
  
  </details>

- **Rendering with SDL2**: the Chip-8's 64x32 screen is implemented by a 960x480 SDL window.

---

## Requirements

- g++ (or any c++ compiler, although modifications to the Makefile will be necessary)
- SDL2 library for graphics
- Makefile for compilation script
- Catch.hpp to run tests

---

## Setup and installation

### [1. Install SDL2 (if not installed)](https://wiki.libsdl.org/SDL2/Installation)

### 2. Clone repository

```shell
git clone https://github.com/guilhermelirar/Chip-8.git
cd Chip-8
```

### 3. Compile program
Use the Makefile provided to compile the program:

```shell
make
```

---

## Usage
To load a Chip-8 program on the emulator, you need to pass the file path as an argument when running from the command line, for example:

```shell
./Chip8 1-chip8-logo.ch8
```

`.ch8` extension is not enforced by this emulator. The emulator will reject a file with size that does not fit in Chip-8 area of memory dedicated to programs,
that is, Chip-8 has a memory of 4 KB, but ROMs are loaded at position 0x200, so a ROM must be at most 3585 bytes.

---

## Tests

This project has automated tests with `catch.hpp`. Tests can be run with the Makefile provided:
```shell
make test
```

---

## Screenshots
<p align="center">
  <img src="images/ibm-logo-rom.png" width="400">
  <img src="images/chip8-logo-rom.png" width="400">
</p>

---

## Inspiration and more information about Chip-8

This is an ongoing personal project for the purpose of fun and learning about emulators. 
I got this idea because I initially thought of an NES emulator, but decided to start with a smaller but still fun project, Chip-8.

In this project, the [Cowgod's Chip-8 Technical Reference](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM#dispcoords) is being a good guide.

---

## License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details
