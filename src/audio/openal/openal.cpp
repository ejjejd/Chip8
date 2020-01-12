#include "openal.h"

namespace audio
{
  bool InitializeOpenal(ALCdevice* device, ALCcontext* context)
  {
    const char* deviceName = alcGetString(nullptr, ALC_DEFAULT_DEVICE_SPECIFIER);
    device = alcOpenDevice(deviceName);

    if(!device)
      return false;

    context = alcCreateContext(device, nullptr);
    alcMakeContextCurrent(context);

    return true;
  }

  void TerminateOpenal(ALCdevice* device, ALCcontext* context)
  {
    alcMakeContextCurrent(nullptr);
    alcDestroyContext(context);
    alcCloseDevice(device);
  }
}
