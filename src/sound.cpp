#include "sound.hpp"
#include <iostream>

Sound::Sound(std::string file): playing(false) {
  if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) == -1) {
    std::cerr << "SDL_mixer could not initialize! SDL_mixer Error: "
              << Mix_GetError() << std::endl;
  }

  chunk = Mix_LoadWAV(file.c_str());
  if (chunk == nullptr) {
    std::cerr << "Failed to load beep sound WAV file" << std::endl;
  }
}

Sound::~Sound() {
  Mix_FreeChunk(chunk);
  chunk = nullptr;
  Mix_CloseAudio();
}

bool Sound::isPlaying() {
  return playing;
}

void Sound::Play() {
  Mix_PlayChannel(-1, chunk, -1);
  playing = true;
}

void Sound::Stop() {
  Mix_HaltChannel(-1);
  playing = false;
}
