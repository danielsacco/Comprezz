#pragma once

#include "IPlug_include_in_plug_hdr.h"
#include "IControls.h"
#include <../../DspLibs/Detectors.h>
#include "../DspLibs/Compressor.h"
#include "PatternMeter.h"

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

enum EControlTags
{
  kCtrlTagGrMeter = 0,
  kCtrlTagScMeter,
  kCtrlTagOutMeter,
  kCtrlTags
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
  IPeakSender<2> grMeterSender;
  IPeakSender<2> scMeterSender;
  IPeakSender<2> outMeterSender;

  std::vector<Compressor> compressors;

  std::vector<DecoupledPeakDetector> scDetectors;
  std::vector<DecoupledPeakDetector> outDetectors;

#endif
};
