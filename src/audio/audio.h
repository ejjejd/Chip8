#include <cstdint>
#include <cmath>
#include <unistd.h>

#include "openal/openal.h"

namespace audio
{
  class Audio
  {
  private:
    static ALCdevice*   ALdevice;
    static ALCcontext*  ALcontext;

    static bool OpenalValid;
  public:
    //Initialize OpenAL
    static void Initialize();

    //Terminate OpenAL
    static void Terminate();

    //Simple Beep function based on Sine Wave
    //Amplitude must be in seconds
    static void Beep(const float frequency, const uint8_t amplitude);
  };
}
