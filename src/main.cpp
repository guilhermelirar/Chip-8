#include "chip8.hpp"
#include <iostream>

int main() {
  std::cout << "Hello, world!\n";
  
  CHIP8 chip8;

  std::cout << "Program Counter at 0x" << std::hex << chip8.cpu.pc << std::endl;
  return 0;
}
