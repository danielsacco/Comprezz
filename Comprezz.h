#pragma once

#include "IPlug_include_in_plug_hdr.h"
#include "IControls.h"
#include <Detector.h>

const int kNumPresets = 1;

enum EParams
{
  kGain = 0,
  kRatio,
  kThreshold,
  kAttack,
  kRelease,
  kNumParams
};

using namespace iplug;
using namespace igraphics;

class Comprezz final : public Plugin
{
public:
  Comprezz(const InstanceInfo& info);
  void OnParamChange(int paramIdx) override;
  void OnReset() override;

#if IPLUG_DSP // http://bit.ly/2S64BDd
  void ProcessBlock(sample** inputs, sample** outputs, int nFrames) override;
  void OnIdle() override;

private:
  IBufferSender<2> mMeterSender;
  std::vector<PeakDetector> detectors;

#endif
};
