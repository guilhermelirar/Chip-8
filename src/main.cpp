#include "chip8.hpp"
#include <iostream>

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cout << "Too few arguments to run CHIP-8 emulador\n"
      << "   Usage: " << argv[0] << " [filename]\n";
    return 0;
  }

  CHIP8 chip8;

  if (chip8.ReadRom(argv[1])) {
    chip8.Run();
  }

  return 0;
}
