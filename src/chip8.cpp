#include "chip8.hpp"
#include "input.hpp"
#include <SDL2/SDL_events.h>
#include <cstring>
#include <fstream>
#include <iostream>

CHIP8::CHIP8() : frameStart(0), interpreter(this), screen(this) {
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

  uint32_t frameStart = SDL_GetTicks();
  uint32_t currentTime;
  uint32_t lastCycleTime = SDL_GetTicks();
  uint32_t cycleInterval = 1000 / 500;

  while (true) {
    currentTime = SDL_GetTicks();
    
    // Rendering and timer decreasin at 60 Hz
    if (currentTime - frameStart >= 16) {
      interpreter.UpdateTimer();
      screen.Render();
      frameStart = currentTime;
      Input::HandleInput();
    }

    // Cycles at 500 Hz
    if (currentTime - lastCycleTime >= cycleInterval) {
      interpreter.RunCycle();
      lastCycleTime = currentTime;
    }

    if (Input::quitRequested) {
      return;
    }

    SDL_Delay(1); // Limit CPU usage
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
