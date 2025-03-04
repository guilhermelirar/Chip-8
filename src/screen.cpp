#include "screen.hpp"
#include "chip8.hpp"
#include <iostream>

#include <cstring>

const int Screen::X_TILES = 64;
const int Screen::Y_TILES = 32;
const int Screen::WIN_WIDTH = X_TILES * 15;
const int Screen::WIN_HEIGHT = Y_TILES * 15;

Screen::Screen(CHIP8 *chip8)
    : window(nullptr), renderer(nullptr), chip8(chip8) {}

Screen::~Screen() {
  if (renderer != nullptr) {
    SDL_DestroyRenderer(renderer);
  }

  if (window != nullptr) {
    SDL_DestroyWindow(window);
  }

  SDL_Quit();

  std::cout << "Screen was destroyed" << std::endl;
}

void Screen::Clear() {
  memset(buffer, false, X_TILES * Y_TILES);
}

void Screen::InitSDL() {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    std::cerr << "Error initializing SDL: " << SDL_GetError() << std::endl;
    exit(EXIT_FAILURE);
  }

  window = SDL_CreateWindow("CHIP-8 Emulator", SDL_WINDOWPOS_CENTERED,
                            SDL_WINDOWPOS_CENTERED, WIN_WIDTH, WIN_HEIGHT, 0);
  if (window == nullptr) {
    std::cerr << "Error creating window: " << SDL_GetError() << std::endl;
    exit(EXIT_FAILURE);
  }

  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (renderer == nullptr) {
    std::cerr << "Error creating renderer: " << SDL_GetError() << std::endl;
    exit(EXIT_FAILURE);
  }
}

void Screen::drawSprite(uint8_t x, uint8_t y, 
                  uint8_t spriteHeight, 
                  uint8_t *sprite) {

  for (uint8_t i = 0; i < spriteHeight; i++) {
    uint8_t spriteLine = sprite[i];
    
    int destY = (y + i) % Y_TILES; // Y where the line will be draw
    
    bool lineBool[8]; 
    for (uint8_t j = 0; j < 8; j++) {
      lineBool[j] = (spriteLine >> (7 - j)) & 0x01;
    }
    
    for (uint8_t j = 0; j < 8; j++) {
      uint8_t destXforPixel = (x + j) % X_TILES; // X position of the j pixel
      uint16_t idxInBuffer = destXforPixel + (destY * X_TILES);

      bool newPixelValue = (buffer[idxInBuffer]^lineBool[j]);
     
      // Collision of pixels
      if (buffer[idxInBuffer] && !newPixelValue) {
        chip8->cpu.V[0xF] = 1;
      }
      buffer[idxInBuffer] = newPixelValue;
    }
  }
}

void Screen::Render() {
  SDL_RenderClear(renderer);
  SDL_RenderPresent(renderer);
  SDL_Delay(16);
}
