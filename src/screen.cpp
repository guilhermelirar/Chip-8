#include "screen.hpp"
#include "chip8.hpp"
#include <iostream>

#include <cstring>

const int Screen::SPRITE_WIDTH = 8;
const int Screen::X_TILES = 64;
const int Screen::Y_TILES = 32;
const int Screen::WIN_WIDTH = X_TILES * 15;
const int Screen::WIN_HEIGHT = Y_TILES * 15;

Screen::Screen(CHIP8 *chip8)
    : window(nullptr), renderer(nullptr), chip8(chip8) {
  Clear();
}

Screen::~Screen() {
  if (renderer) {
    SDL_DestroyRenderer(renderer);
    renderer = nullptr;
  }
  if (window) {
    SDL_DestroyWindow(window);
    window = nullptr;
  }

  SDL_Quit();

  std::cout << "Screen was destroyed" << std::endl;
}

void Screen::Clear() {
  memset(buffer, false, X_TILES * Y_TILES);
}

void Screen::InitSDL() {
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
    std::cerr << "Error initializing SDL: " << SDL_GetError() << std::endl;
    exit(EXIT_FAILURE);
  }

  window = SDL_CreateWindow("CHIP-8 Emulator", SDL_WINDOWPOS_CENTERED,
                            SDL_WINDOWPOS_CENTERED, WIN_WIDTH, WIN_HEIGHT, 0);
  if (window == nullptr) {
    std::cerr << "Error creating window: " << SDL_GetError() << std::endl;
    exit(EXIT_FAILURE);
  }

  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
  if (renderer == nullptr) {
    std::cerr << "Error creating renderer: " << SDL_GetError() << std::endl;
    exit(EXIT_FAILURE);
  }
}

void Screen::drawSprite(uint8_t x, uint8_t y, uint8_t spriteHeight,
                        uint8_t *sprite) {

  y %= Y_TILES;
 
  uint8_t maxHeight = std::min<uint8_t>(spriteHeight, Y_TILES - y);
  uint8_t maxWidth = std::min<uint8_t>(SPRITE_WIDTH, X_TILES - x);

  for (uint8_t i = 0; i < spriteHeight; i++) {
    // Checking if the line has pixels outside screen
    uint8_t partiallyOutOfRightSide = ((0xFF >> maxWidth) & sprite[i]);
    bool partiallyOutOfBottom = (i > maxHeight) && (sprite[i] != 0);
    if (partiallyOutOfRightSide || partiallyOutOfBottom) {
      return;
    }
  }

  for (uint8_t i = 0; i < maxHeight; i++) {
    uint8_t spriteLine = sprite[i];

    int destY = (y + i); // Y where the line will be draw

    for (uint8_t j = 0; j < maxWidth; j++) {
      uint8_t destXforPixel = (x + j) % X_TILES; // X position of the j pixel
      uint16_t idxInBuffer = destXforPixel + (destY * X_TILES);

      bool newPixelValue =
          (buffer[idxInBuffer] ^ ((spriteLine >> (7 - j)) & 1));

      // Collision of pixels
      if (buffer[idxInBuffer] && !newPixelValue) {
        chip8->interpreter.V[0xF] = 1;
      }
      buffer[idxInBuffer] = newPixelValue;
    }
  }
}

void Screen::Render() {
  // Clears screen with black
  SDL_SetRenderDrawColor(renderer, 15, 15, 40, 255);
  SDL_RenderClear(renderer);

  // Rect used to draw "pixels"
  SDL_Rect rect = {0, 0, WIN_WIDTH / X_TILES, WIN_HEIGHT / Y_TILES};

  // For each pixel
  for (uint16_t pixel = 0; pixel < X_TILES * Y_TILES; pixel++) {
    // Coordinates in 64x32 grid
    int y = pixel / X_TILES;
    int x = pixel % X_TILES;
    
    // Paint if true
    if (buffer[pixel]) {
      rect.x = x * WIN_WIDTH / X_TILES; 
      rect.y = y * WIN_HEIGHT / Y_TILES; 
      SDL_SetRenderDrawColor(renderer, 0, 255, 102, 255);
      SDL_RenderFillRect(renderer, &rect);
    }
  }

  SDL_RenderPresent(renderer);
}
