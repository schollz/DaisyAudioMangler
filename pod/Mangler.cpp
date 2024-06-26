#include "../audio.h"
#include "daisy_pod.h"
#include <limits.h>
#include <string.h>

using namespace daisy;

DaisyPod hw;

#define nelem(x) (sizeof(x) / sizeof(*x))

float DSY_SDRAM_BSS
    buffer[(1 << 26) / sizeof(float)]; /* Use all 64MB of sample RAM */

static void AudioCallback(AudioHandle::InterleavingInputBuffer in,
                          AudioHandle::InterleavingOutputBuffer out,
                          size_t size) {
  hw.ProcessAllControls();

  int encoder = hw.encoder.Increment();
  if (encoder) {
    if (hw.button1.Pressed())
      buf_setdirection(encoder);
  }

  if (hw.button1.RisingEdge())
    buf_setmode(BUF_VARISPEED);
  else if (hw.button2.RisingEdge())
    buf_setmode(BUF_STOP);
  else if (hw.button1.FallingEdge() || hw.button2.FallingEdge())
    buf_setmode(BUF_PASSTHROUGH);

  buf_setspeed(hw.knob1.Process());
  buf_callback(in, out, size);
}

int main(void) {
  hw.Init();
  hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_96KHZ);
  buf_init(buffer, nelem(buffer), hw.AudioSampleRate());
  hw.StartAdc();
  hw.StartAudio(AudioCallback);

  while (1)
    ;
}
