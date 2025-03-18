#include "chip8.hpp"
#include "input.hpp"
#include <SDL2/SDL_events.h>
#include <cstring>
#include <fstream>
#include <iostream>

CHIP8::CHIP8() : interpreter(this), screen(this) {
  // Initializing font data
  uint8_t fontData[] = {
      0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
      0x20, 0x60, 0x20, 0x20, 0x70, // 1
      0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
      0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
      0x90, 0x90, 0xF0, 0x10, 0x10, // 4
      0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
      0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
      0xF0, 0x10, 0x20, 0x40, 0x40, // 7
      0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
      0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
      0xF0, 0x90, 0xF0, 0x90, 0x90, // A
      0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
      0xF0, 0x80, 0x80, 0x80, 0xF0, // C
      0xE0, 0x90, 0x90, 0x90, 0xE0, // D
      0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
      0xF0, 0x80, 0xF0, 0x80, 0x80  // F
  };

  memcpy(&memory[0x50], fontData, sizeof(fontData));
}

void CHIP8::Run() {
  screen.InitSDL();
  while (true) {
    screen.Render();

    Input::HandleInput();
    interpreter.RunCycle();
    
    if (Input::quitRequested) {
      return;
    }
  }
}

bool CHIP8::ReadRom(const char *filename) {
  std::fstream file;

  file.open(filename, std::ios::in | std::ios::binary);

  if (!file) {
    std::cout << "Error while opening the file\n";
    return false;
  }

  file.seekg(0, std::ios::end);
  size_t fileSize = file.tellg();
  file.seekg(0, std::ios::beg);

  if (fileSize > (size_t)(MEMORY_SIZE - interpreter.pc)) {
    std::cout << "ROM is too large to fit on CHIP-8 memory (max file size is "
                 "3584 bytes)\n";
    return false;
  }

  file.read(reinterpret_cast<char *>(&memory[interpreter.pc]), fileSize);
  
  if (!file) {
    std::cout << "Error while reading the file\n";
    return false;
  }

  return true;
}
