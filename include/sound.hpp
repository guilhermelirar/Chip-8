#ifndef SOUND_HPP
#define SOUND_HPP

#include <SDL2/SDL_mixer.h>
#include <string>

class Sound {
 public:
  Sound(std::string file);
  ~Sound();

  bool isPlaying();
  void Play();
  void Stop();
 private:
  bool playing;
  Mix_Chunk* chunk;
};

#endif // SOUND_HPP
