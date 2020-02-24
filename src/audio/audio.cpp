#include "audio.h"

namespace audio
{
  void Audio::Initialize()
  {
    OpenalValid = InitializeOpenal(ALdevice, ALcontext);
  }

  void Audio::Terminate()
  {
    TerminateOpenal(ALdevice, ALcontext);
  }

  void Audio::Beep(const float frequency, const uint8_t amplitude)
  {
    if(!OpenalValid)
      return;

    uint16_t sampleRate = 44'100;
    uint64_t bufferSize = amplitude * sampleRate;

    uint16_t* samples = new uint16_t[bufferSize];
    if(!samples)
      return;

    for(int i = 0; i < bufferSize; ++i)
      samples[i] = 32'760 * sin((2.0f * (float)M_PI * frequency) / sampleRate * i);

    ALuint alBuffer;
    alGenBuffers(1, &alBuffer);
    alBufferData(alBuffer, AL_FORMAT_MONO16, samples, bufferSize, sampleRate);

    ALuint src;
    alGenSources(1, &src);
    alSourcei(src, AL_BUFFER, alBuffer);
    alSourcePlay(src);

    sleep(amplitude);

    delete[] samples;
  }

  ALCdevice* Audio::ALdevice    = nullptr;
  ALCcontext* Audio::ALcontext  = nullptr;

  bool Audio::OpenalValid = false;
}
