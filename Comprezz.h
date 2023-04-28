#pragma once

#include "IPlug_include_in_plug_hdr.h"
#include "IControls.h"
#include <../../DspLibs/Detectors.h>
#include "../DspLibs/Compressor.h"
#include "PatternMeter.h"
#include "DelayLine.h"
#include "delay_line.h"

const int kNumPresets = 2;

enum EParams
{
  kGain = 0,
  kRatio,
  kThreshold,
  kAttackMs,      // Attack time in ms
  kReleaseMs,     // Release time in ms
  kStereoLink,
  kLookAhead,
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
  double linearGain = 0.;

  IPeakSender<2> grMeterSender;
  IPeakSender<2> scMeterSender;
  IPeakSender<2> outMeterSender;

  std::array<DelayLine<8192>, 2> delays;

  std::vector<Compressor> compressors;

  std::vector<DecoupledPeakDetector> scDetectors;
  std::vector<DecoupledPeakDetector> outDetectors;

  void ClearDelaySamples();
  void UpdateDelaySamples();

#endif
};
