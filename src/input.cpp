#include "input.hpp"
#include <SDL2/SDL.h>

bool Input::keyState[16] = {false};

void Input::HandleInput() {
  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    switch (e.type) {
    case SDL_KEYDOWN:
    case SDL_KEYUP: {
      bool isPressed = (e.type == SDL_KEYDOWN);
      switch (e.key.keysym.scancode) {
      case SDL_SCANCODE_1:
        keyState[0x1] = isPressed;
        break;
      case SDL_SCANCODE_2:
        keyState[0x2] = isPressed;
        break;
      case SDL_SCANCODE_3:
        keyState[0x3] = isPressed;
        break;
      case SDL_SCANCODE_4:
        keyState[0xC] = isPressed;
        break;
      case SDL_SCANCODE_Q:
        keyState[0x4] = isPressed;
        break;
      case SDL_SCANCODE_W:
        keyState[0x5] = isPressed;
        break;
      case SDL_SCANCODE_E:
        keyState[0x6] = isPressed;
        break;
      case SDL_SCANCODE_R:
        keyState[0xD] = isPressed;
        break;
      case SDL_SCANCODE_A:
        keyState[0x7] = isPressed;
        break;
      case SDL_SCANCODE_S:
        keyState[0x8] = isPressed;
        break;
      case SDL_SCANCODE_D:
        keyState[0x9] = isPressed;
        break;
      case SDL_SCANCODE_F:
        keyState[0xE] = isPressed;
        break;
      case SDL_SCANCODE_Z:
        keyState[0xA] = isPressed;
        break;
      case SDL_SCANCODE_X:
        keyState[0x0] = isPressed;
        break;
      case SDL_SCANCODE_C:
        keyState[0xB] = isPressed;
        break;
      case SDL_SCANCODE_V:
        keyState[0xF] = isPressed;
        break;
      default:
        break;
      }
    } break;
    }
  }
}

bool Input::IsKeyDown(uint8_t key) {
  if (key > 15) {
    return false;
  }

  return keyState[key];
}
