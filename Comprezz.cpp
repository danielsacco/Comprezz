#include "Comprezz.h"
#include "IPlug_include_in_plug_src.h"
#include "IControls.h"

Comprezz::Comprezz(const InstanceInfo& info)
: Plugin(info, MakeConfig(kNumParams, kNumPresets))
{
  GetParam(kGain)->InitGain("Gain", 0., -3., 12., .5);
  GetParam(kRatio)->InitDouble("Ratio", 2., 1., 10., .1);
  GetParam(kThreshold)->InitDouble("Threshold", -30., -60., 0., .1, "dB");
  GetParam(kAttack)->InitDouble("Attack", 10., 1., 100., .1, "ms");
  GetParam(kRelease)->InitDouble("Release", 100., 50., 1000., 1., "ms");
  GetParam(kStereoLink)->InitBool("Link", false);


#if IPLUG_EDITOR // http://bit.ly/2S64BDd
  mMakeGraphicsFunc = [&]() {
    return MakeGraphics(*this, PLUG_WIDTH, PLUG_HEIGHT, PLUG_FPS, GetScaleForScreen(PLUG_WIDTH, PLUG_HEIGHT));
  };
  
  mLayoutFunc = [&](IGraphics* pGraphics) {
    pGraphics->AttachCornerResizer(EUIResizerMode::Scale, false);
    pGraphics->AttachPanelBackground(COLOR_MID_GRAY);
    pGraphics->LoadFont("Roboto-Regular", ROBOTO_FN);
    const IRECT fullUI = pGraphics->GetBounds();

    // TODO Improve this section !!!
    const int columns = 9;
    int nextColumn = 0;
    const IRECT ratioColumn = fullUI.GetGridCell(0, nextColumn++, 1, columns);
    const IRECT thresholdColumn = fullUI.GetGridCell(0, nextColumn++, 1, columns);
    const IRECT attackColumn = fullUI.GetGridCell(0, nextColumn++, 1, columns);
    const IRECT releaseColumn = fullUI.GetGridCell(0, nextColumn++, 1, columns);
    const IRECT grColumn = fullUI.GetGridCell(0, nextColumn++, 1, columns);
    const IRECT scColumn = fullUI.GetGridCell(0, nextColumn++, 1, columns);
    const IRECT outColumn = fullUI.GetGridCell(0, nextColumn++, 1, columns);
    const IRECT gainColumn = fullUI.GetGridCell(0, nextColumn++, 1, columns);
    const IRECT stereoLinkColumn = fullUI.GetGridCell(0, nextColumn++, 1, columns);

    pGraphics->AttachControl(new IVKnobControl(ratioColumn.GetCentredInside(100), kRatio));
    pGraphics->AttachControl(new IVKnobControl(thresholdColumn.GetCentredInside(100), kThreshold));
    pGraphics->AttachControl(new IVKnobControl(attackColumn.GetCentredInside(100), kAttack));
    pGraphics->AttachControl(new IVKnobControl(releaseColumn.GetCentredInside(100), kRelease));

    pGraphics->AttachControl(new IVInvertedPatternMeterControl<2>(grColumn, "GR", DEFAULT_STYLE.WithColor(kX2, COLOR_GREEN).WithColor(kX3, COLOR_RED), EDirection::Vertical, { "L", "R" }, 0, IVMeterControl<2>::EResponse::Log, -72.f, 0.f), kCtrlTagGrMeter);
    pGraphics->AttachControl(new IVPatternMeterControl<2>{scColumn, "SC Level", DEFAULT_STYLE.WithColor(kX2, COLOR_GREEN).WithColor(kX3, COLOR_RED), EDirection::Vertical, { "L", "R" }, 0, IVMeterControl<2>::EResponse::Log }, kCtrlTagScMeter);
    pGraphics->AttachControl(new IVPatternMeterControl<2>(outColumn, "Out Level", DEFAULT_STYLE.WithColor(kX2, COLOR_GREEN).WithColor(kX3, COLOR_RED), EDirection::Vertical, { "L", "R" }, 0, IVMeterControl<2>::EResponse::Log), kCtrlTagOutMeter);

    pGraphics->AttachControl(new IVKnobControl(gainColumn.GetCentredInside(100), kGain));

    pGraphics->AttachControl(new IVToggleControl(stereoLinkColumn.GetCentredInside(50), kStereoLink));

  };
#endif
}

void Comprezz::OnParamChange(int paramIdx)
{
  switch (paramIdx)
  {
  case kGain:
  {
    linearGain = DBToAmp(GetParam(kGain)->Value());
    break;
  }
  case kAttack:
    {
      for (auto &compressor : compressors)
        compressor.SetAttack(GetParam(kAttack)->Value());
      break;
    }
    case kRelease:
    {
      for (auto &compressor : compressors)
        compressor.SetRelease(GetParam(kRelease)->Value());
      break;
    }
    case kRatio:
    {
      for (auto& compressor : compressors)
        compressor.SetRatio(GetParam(kRatio)->Value());
      break;
    }
    case kThreshold:
    {
      for (auto& compressor : compressors)
        compressor.SetThreshold(GetParam(kThreshold)->Value());
      break;
    }
  }
}

void Comprezz::OnReset()
{
  for (auto &compressor : compressors)
  {
    compressor.SetSampleRate(GetSampleRate());
  }
}

#if IPLUG_DSP
void Comprezz::ProcessBlock(sample** inputs, sample** outputs, int nFrames)
{
  const int nChans = NOutChansConnected();

  // Create compressors if needed (first time or nChans have changed)
  if (compressors.size() != nChans)
  {
    compressors.clear();
    for (int i = 0; i < nChans; i++)
    {
      compressors.push_back(Compressor(
        GetParam(kThreshold)->Value(),
        GetParam(kRatio)->Value(),
        0.,
        GetSampleRate(),
        GetParam(kAttack)->Value(),
        GetParam(kRelease)->Value()));
    }
  }

  // Create detectors if needed (first time or nChans have changed)
  if (scDetectors.size() != nChans)
  {
    scDetectors.clear();
    for (int i = 0; i < nChans; i++)
    {
      scDetectors.push_back(DecoupledPeakDetector(
        GetSampleRate(),
        GetParam(kAttack)->Value(),
        GetParam(kRelease)->Value()));
    }
  }

  if (outDetectors.size() != nChans)
  {
    outDetectors.clear();
    for (int i = 0; i < nChans; i++)
    {
      outDetectors.push_back(DecoupledPeakDetector(
        GetSampleRate(),
        GetParam(kAttack)->Value(),
        GetParam(kRelease)->Value()));
    }
  }

  // Horrible: Allocate arrays for meters
  double** vcaMeter = new double*[nChans];
  double** scMeter = new double* [nChans];
  double** outMeter = new double* [nChans];
  for (int i = 0; i < nChans; i++) {
    vcaMeter[i] = new double[nFrames];
    scMeter[i] = new double[nFrames];
    outMeter[i] = new double[nFrames];
  }

  double* sidechain = nullptr;

  if (GetParam(kStereoLink)->Bool() && nChans > 1)
  {
    // Prepare channels average as sidechain
    sidechain = new double [nFrames];

    for (int s = 0; s < nFrames; s++)
    {
      sample average = 0.;
      for (int i = 0; i < nChans; i++)
      {
        average += std::abs(inputs[i][s]);
      }
      sidechain[s] = average / nChans;
    }
  }

  // Process signal thru compressors
  for (int i = 0; i < nChans; i++)
  {
    (&compressors[i])->ProcessBlock(inputs[i], sidechain, outputs[i], vcaMeter[i], nFrames);

    // Apply output gain
    for (int s = 0; s < nFrames; s++)
    {
      outputs[i][s] = outputs[i][s] * linearGain;
    }
  }

  // Fill sc and out meters
  for (int i = 0; i < nChans; i++)
  {
    auto scDetector = &(scDetectors[i]);
    auto outDetector = &(outDetectors[i]);
    for (int s = 0; s < nFrames; s++) {
      scMeter[i][s] = scDetector->ProcessSample(inputs[i][s]);
      outMeter[i][s] = outDetector->ProcessSample(outputs[i][s]);
    }
  }



  // Send data to meters
  grMeterSender.ProcessBlock(vcaMeter, nFrames, kCtrlTagGrMeter);
  scMeterSender.ProcessBlock(scMeter, nFrames, kCtrlTagScMeter);
  outMeterSender.ProcessBlock(outMeter, nFrames, kCtrlTagOutMeter);

  // Horrible: Deallocate arrays
  for (int i = 0; i < nChans; i++) {
    delete[] vcaMeter[i];
    delete[] scMeter[i];
    delete[] outMeter[i];
  }
  delete[] vcaMeter;
  delete[] scMeter;
  delete[] outMeter;

  if (sidechain)
  {
    delete[] sidechain;
  }
}

void Comprezz::OnIdle()
{
  grMeterSender.TransmitData(*this);
  scMeterSender.TransmitData(*this);
  outMeterSender.TransmitData(*this);
}

#endif
