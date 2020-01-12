#include "al.h"
#include "alc.h"

namespace audio
{
  bool InitializeOpenal(ALCdevice* device, ALCcontext* context);
  void TerminateOpenal(ALCdevice* device, ALCcontext* context);
}
