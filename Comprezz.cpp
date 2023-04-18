#include "Comprezz.h"
#include "IPlug_include_in_plug_src.h"
#include "IControls.h"

Comprezz::Comprezz(const InstanceInfo& info)
: Plugin(info, MakeConfig(kNumParams, kNumPresets))
{
  GetParam(kGain)->InitDouble("Gain", 0., 0., 100.0, 0.01, "%");
  GetParam(kRatio)->InitDouble("Ratio", 2., 1., 10., .1);
  GetParam(kThreshold)->InitDouble("Threshold", -30., -60., 0., .1, "dB");
  GetParam(kAttack)->InitDouble("Attack", 2., 1., 100., .1, "ms");
  GetParam(kRelease)->InitDouble("Release", 100., 50., 1000., 1., "ms");

#if IPLUG_EDITOR // http://bit.ly/2S64BDd
  mMakeGraphicsFunc = [&]() {
    return MakeGraphics(*this, PLUG_WIDTH, PLUG_HEIGHT, PLUG_FPS, GetScaleForScreen(PLUG_WIDTH, PLUG_HEIGHT));
  };
  
  mLayoutFunc = [&](IGraphics* pGraphics) {
    pGraphics->AttachPanelBackground(COLOR_GRAY);
    pGraphics->LoadFont("Roboto-Regular", ROBOTO_FN);
    const IRECT fullUI = pGraphics->GetBounds();

    const int columns = 6;
    int nextColumn = 0;
    const IRECT ratioColumn = fullUI.GetGridCell(0, nextColumn++, 1, columns);
    const IRECT thresholdColumn = fullUI.GetGridCell(0, nextColumn++, 1, columns);
    const IRECT attackColumn = fullUI.GetGridCell(0, nextColumn++, 1, columns);
    const IRECT releaseColumn = fullUI.GetGridCell(0, nextColumn++, 1, columns);
    const IRECT displayColumn = fullUI.GetGridCell(0, nextColumn++, 1, columns);
    const IRECT gainColumn = fullUI.GetGridCell(0, nextColumn++, 1, columns);

    pGraphics->AttachControl(new IVKnobControl(ratioColumn.GetCentredInside(100), kRatio));
    pGraphics->AttachControl(new IVKnobControl(thresholdColumn.GetCentredInside(100), kThreshold));
    pGraphics->AttachControl(new IVKnobControl(attackColumn.GetCentredInside(100), kAttack));
    pGraphics->AttachControl(new IVKnobControl(releaseColumn.GetCentredInside(100), kRelease));

    pGraphics->AttachControl(new IVMeterControl<2>(displayColumn, "Gain Reduction", DEFAULT_STYLE, EDirection::Vertical, { "L", "R" }), kGain);

    pGraphics->AttachControl(new IVKnobControl(gainColumn.GetCentredInside(100), kGain));

  };
#endif
}

void Comprezz::OnParamChange(int paramIdx)
{
  switch (paramIdx)
  {
    case kAttack:
    {
      for (auto &detector : detectors)
      {
        detector.setAttack(GetParam(kAttack)->Value());
      }
      break;
    }
    case kRelease:
    {
      for (auto &detector : detectors)
      {
        detector.setRelease(GetParam(kRelease)->Value());
      }
      break;
    }
  }
}

void Comprezz::OnReset()
{
  for (auto detector : detectors)
  {
    detector.setSampleRate(GetSampleRate());
  }
}

#if IPLUG_DSP
void Comprezz::ProcessBlock(sample** inputs, sample** outputs, int nFrames)
{
  const double gain = GetParam(kGain)->Value() / 100.;
  const int nChans = NOutChansConnected();

  // Create Peak Detectors if needed
  if (detectors.size() != nChans)
  {
    detectors.clear();
    double sampleRate = GetSampleRate();
    double attack = GetParam(kAttack)->Value();
    double release = GetParam(kRelease)->Value();
    for (int i = 0; i < nChans; i++)
    {
      detectors.push_back(PeakDetector(sampleRate, attack, release));
    }
  }


  // log of input signal
  for (int ch = 0; ch < nChans; ch++) {
    for (int s = 0; s < nFrames; s++) {
      auto logSample = AmpToDB(inputs[ch][s]);
      outputs[ch][s] = logSample;
    }
  }

  // TODO: Pass log signal through gain curve
  // Here we have the input signal converted to dBs between -infinite and zero (or greater)
  const double threshold = GetParam(kThreshold)->Value();
  const double ratio = GetParam(kRatio)->Value();
  for (int ch = 0; ch < nChans; ch++) {
    for (int s = 0; s < nFrames; s++) {
      double sample = outputs[ch][s];

      if (sample > threshold)
      {
        double delta = sample - threshold;
        outputs[ch][s] = - (delta * (1. - 1. / ratio));   // Gain Reduction in dBs
      }
      else
      {
        //outputs[ch][s] = -DBL_MAX;
        outputs[ch][s] = 0;
      }
    }
  }

  // Back to linear for attacking the detector
  for (int ch = 0; ch < nChans; ch++) {
    for (int s = 0; s < nFrames; s++) {
      double linearValue = DBToAmp(outputs[ch][s]);
      outputs[ch][s] = linearValue;
    }
  }

  // Attack/Release post gain curve
  for (int ch = 0; ch < nChans; ch++) {
    auto detector = &(detectors[ch]);

    for (int s = 0; s < nFrames; s++) {
      outputs[ch][s] = 1-detector->ProcessSample(1-outputs[ch][s]);
    }
  }

  mMeterSender.ProcessBlock(outputs, nFrames, kGain);


  // Reduce gain
  for (int ch = 0; ch < nChans; ch++) {
    for (int s = 0; s < nFrames; s++) {
      outputs[ch][s] = inputs[ch][s] * outputs[ch][s];
    }
  }

}

void Comprezz::OnIdle()
{
  mMeterSender.TransmitData(*this);
}

#endif
