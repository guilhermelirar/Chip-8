#ifndef SCREEN_HPP
#define SCREEN_HPP

#include <cstdint>
#include <SDL2/SDL_image.h>

class CHIP8;

class Screen {
private:
  SDL_Window *window;
  SDL_Renderer *renderer;
public:
  static const int X_TILES, Y_TILES;
  static const int WIN_WIDTH, WIN_HEIGHT;

  CHIP8 *chip8;         // CHIP-8 System
  bool buffer[0x800];   // Pixel buffer

  Screen(CHIP8 *chip8); // Constructor
  ~Screen();            // Destructor 

  // Initializing SDL Subsystems
  void InitSDL();

  // Rendering window
  void Render();

  void Clear();         // Clears display
  
  // Draws a sprite of certain height,
  // at coordinates x and y
  void drawSprite(uint8_t x, uint8_t y, 
                  uint8_t spriteHeight, 
                  uint8_t *sprite);

};

#endif // SCREEN_HPP
