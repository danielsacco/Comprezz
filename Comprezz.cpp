#include "Comprezz.h"
#include "IPlug_include_in_plug_src.h"
#include "IControls.h"

Comprezz::Comprezz(const InstanceInfo& info)
: Plugin(info, MakeConfig(kNumParams, kNumPresets))
{
  GetParam(kGain)->InitGain("Gain", 0., -3., 12., .5);
  GetParam(kRatio)->InitDouble("Ratio", 1.3, 1., 6., .1);
  GetParam(kThreshold)->InitDouble("Threshold", -30., -60., 0., .1, "dB");
  GetParam(kAttackMs)->InitDouble("Attack", 10., 1., 100., .1, "ms");
  GetParam(kReleaseMs)->InitDouble("Release", 100., 50., 1000., 1., "ms");
  GetParam(kStereoLink)->InitBool("Link Channels", false);
  GetParam(kLookAhead)->InitBool("Look Ahead", false);
  GetParam(kKneeWidth)->InitDouble("Knee Width", 0., 0., 10., .1, "dB");

  // Factory Presets
  // Params in order: kGain, kRatio, kThreshold, kAttackMs, kReleaseMs, kStereoLink, kLookAhead, kKneeWidth
  MakePreset("Bass Attack", 6., 5., -20., 30., 300., true, false, 4.);
  MakePreset("Kick Drum Sustain", 6. , 2.5, -20., 10., 50., true, true, 0.);
  MakePreset("Tight Program Compression", 12., 6., -18., 15., 75., true, true, 6.);

#if IPLUG_EDITOR // http://bit.ly/2S64BDd
  mMakeGraphicsFunc = [&]() {
    return MakeGraphics(*this, PLUG_WIDTH, PLUG_HEIGHT, PLUG_FPS, GetScaleForScreen(PLUG_WIDTH, PLUG_HEIGHT));
  };

  mLayoutFunc = [&](IGraphics* pGraphics) {
    pGraphics->AttachCornerResizer(EUIResizerMode::Scale, false);
    pGraphics->AttachPanelBackground(COLOR_MID_GRAY);
    pGraphics->LoadFont("Roboto-Regular", ROBOTO_FN);
    pGraphics->LoadFont("ForkAwesome", FORK_AWESOME_FN);

    const float HEADER_HEIGHT = 30.f;
    const float FOOTER_HEIGHT = 10.f;
    const float AVAILABLE_HEIGHT = PLUG_HEIGHT - (HEADER_HEIGHT + FOOTER_HEIGHT);
    const float BOTOM_CONTROLS_HEIGHT = 30.f;

    const float CONTROLS_HEIGHT = AVAILABLE_HEIGHT - BOTOM_CONTROLS_HEIGHT;
    const float CONTROLS_WIDTH = 50.f;
    const float METERS_WIDTH = 40.f;
    const IRECT CONTROLS_SIZE = IRECT(0.f, 0.f, CONTROLS_WIDTH, CONTROLS_HEIGHT);
    const IRECT METERS_SIZE = IRECT(0.f, 0.f, METERS_WIDTH, CONTROLS_HEIGHT);

    const IColor METER_FRAME_COLOR = IColor::LinearInterpolateBetween(COLOR_LIGHT_GRAY, COLOR_MID_GRAY, .5f);

    const IText LABEL_TEXT = DEFAULT_TEXT.WithSize(14.f);
    const IVStyle CUSTOM_STYLE = DEFAULT_STYLE.WithLabelText(LABEL_TEXT);
    const IVStyle METERS_STYLE = CUSTOM_STYLE
      .WithColor(kBG, COLOR_RED)            // Not used
      .WithColor(kFG, COLOR_BLUE)
      .WithColor(kPR, COLOR_VIOLET)
      .WithColor(kFR, METER_FRAME_COLOR)    // Frame
      .WithColor(kHL, METER_FRAME_COLOR)    // dB Markers
      .WithColor(kSH, COLOR_YELLOW)
      .WithColor(kX1, COLOR_INDIGO)
      .WithColor(kX2, COLOR_GREEN)          // Track Pattern main color
      .WithColor(kX3, COLOR_RED)            // Track Pattern danger color
      ;

    const IRECT fullUI = pGraphics->GetBounds();
    const IRECT header = fullUI.GetFromTop(HEADER_HEIGHT);
    const IRECT mainControls = fullUI.GetFromTop(CONTROLS_HEIGHT).GetVShifted(HEADER_HEIGHT);
    const IRECT bottomControls = fullUI.GetFromBottom(BOTOM_CONTROLS_HEIGHT).GetVShifted(-FOOTER_HEIGHT);

    pGraphics->AttachControl(new ITextControl(header, "Just Another Basic Digital Compressor", DEFAULT_TEXT.WithSize(20.f)));

    // TODO Improve this section !!!
    const int columns = 10;
    int nextColumn = 0;
    const IRECT ratioColumn = mainControls.GetGridCell(0, nextColumn++, 1, columns);
    const IRECT thresholdColumn = mainControls.GetGridCell(0, nextColumn++, 1, columns);
    const IRECT kneeColumn = mainControls.GetGridCell(0, nextColumn++, 1, columns);
    const IRECT scColumn = mainControls.GetGridCell(0, nextColumn++, 1, columns);
    const IRECT attackColumn = mainControls.GetGridCell(0, nextColumn++, 1, columns);
    const IRECT releaseColumn = mainControls.GetGridCell(0, nextColumn++, 1, columns);
    const IRECT grColumn = mainControls.GetGridCell(0, nextColumn++, 1, columns);
    const IRECT gainColumn = mainControls.GetGridCell(0, nextColumn++, 1, columns);
    const IRECT outColumn = mainControls.GetGridCell(0, nextColumn++, 1, columns);

    pGraphics->AttachControl(new IVSliderControl(ratioColumn.GetCentredInside(CONTROLS_SIZE), kRatio, "Ratio", CUSTOM_STYLE, true));
    pGraphics->AttachControl(new IVSliderControl(thresholdColumn.GetCentredInside(CONTROLS_SIZE), kThreshold, "Thr", CUSTOM_STYLE, true));
    pGraphics->AttachControl(new IVSliderControl(kneeColumn.GetCentredInside(CONTROLS_SIZE), kKneeWidth, "Knee", CUSTOM_STYLE, true));
    pGraphics->AttachControl(new IVPatternMeterControl<2>{ scColumn.GetCentredInside(METERS_SIZE), "In/SC", METERS_STYLE, EDirection::Vertical, {}, 0, IVMeterControl<>::EResponse::Log }, kCtrlTagScMeter);
    pGraphics->AttachControl(new IVSliderControl(attackColumn.GetCentredInside(CONTROLS_SIZE), kAttackMs, "Attack", CUSTOM_STYLE, true));
    pGraphics->AttachControl(new IVSliderControl(releaseColumn.GetCentredInside(CONTROLS_SIZE), kReleaseMs, "Release", CUSTOM_STYLE, true));
    pGraphics->AttachControl(new IVInvertedPatternMeterControl<2>(grColumn.GetCentredInside(METERS_SIZE), "GR", METERS_STYLE, EDirection::Vertical, {}, 0), kCtrlTagGrMeter);
    pGraphics->AttachControl(new IVSliderControl(gainColumn.GetCentredInside(CONTROLS_SIZE), kGain, "Make Up", CUSTOM_STYLE, true));
    pGraphics->AttachControl(new IVPatternMeterControl<2>(outColumn.GetCentredInside(METERS_SIZE), "Out", METERS_STYLE, EDirection::Vertical, {}, 0), kCtrlTagOutMeter);

    const IText forkAwesomeText{ 16.f, "ForkAwesome" };

    const IRECT linkCheckbox = bottomControls.SubRectHorizontal(columns, 0).GetHAlignedTo(grColumn, EAlign::Center).GetCentredInside(25.f);
    pGraphics->AttachControl(new ITextToggleControl(linkCheckbox, kStereoLink, u8"\uf096", u8"\uf14a", forkAwesomeText));

    const IRECT linkLabel = IRECT(linkCheckbox.L - 80.f, bottomControls.T, linkCheckbox.L, bottomControls.B);
    pGraphics->AttachControl(new ITextControl(linkLabel, "Link Channels", LABEL_TEXT.WithAlign(EAlign::Far)));

    const IRECT lookAheadCheckbox = bottomControls.SubRectHorizontal(columns, 0).GetHAlignedTo(scColumn, EAlign::Center).GetCentredInside(25.f);
    pGraphics->AttachControl(new ITextToggleControl(lookAheadCheckbox, kLookAhead, u8"\uf096", u8"\uf14a", forkAwesomeText));

    const IRECT lookAheadLabel = IRECT(lookAheadCheckbox.L - 80.f, bottomControls.T, lookAheadCheckbox.L, bottomControls.B);
    pGraphics->AttachControl(new ITextControl(lookAheadLabel, "Look Ahead", LABEL_TEXT.WithAlign(EAlign::Far)));


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
  case kLookAhead:
  {
    if (GetParam(kLookAhead)->Bool()) {
      UpdateDelaySamples();
    }
    else {
      ClearDelaySamples();
    }
    break;
  }
  case kAttackMs:
    {
      double attackTimeMs = GetParam(kAttackMs)->Value();
      for (auto& compressor : compressors)
        compressor.SetAttackTime(attackTimeMs / 1000.);

      if (GetParam(kLookAhead)->Bool()) {
        UpdateDelaySamples();
      }
      break;
    }
    case kReleaseMs:
    {
      for (auto &compressor : compressors)
        compressor.SetReleaseTime(GetParam(kReleaseMs)->Value() / 1000.);
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
    case kKneeWidth:
    {
      for (auto& compressor : compressors)
        compressor.SetKneeWidth(GetParam(kKneeWidth)->Value());
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
      compressors.push_back(dsptk::Compressor(
        GetParam(kThreshold)->Value(),
        GetParam(kRatio)->Value(),
        GetParam(kKneeWidth)->Value(),
        GetSampleRate(),
        GetParam(kAttackMs)->Value() / 1000,
        GetParam(kReleaseMs)->Value() / 1000));
    }
  }

  // Create detectors if needed (first time or nChans have changed)
  if (scDetectors.size() != nChans)
  {
    scDetectors.clear();
    for (int i = 0; i < nChans; i++)
    {
      scDetectors.push_back(dsptk::DecoupledPeakDetector(
        GetSampleRate(),
        GetParam(kAttackMs)->Value() / 1000.,
        GetParam(kReleaseMs)->Value() / 1000.));
    }
  }

  if (outDetectors.size() != nChans)
  {
    outDetectors.clear();
    for (int i = 0; i < nChans; i++)
    {
      outDetectors.push_back(dsptk::DecoupledPeakDetector(
        GetSampleRate(),
        GetParam(kAttackMs)->Value() / 1000,
        GetParam(kReleaseMs)->Value() / 1000));
    }
  }

  // Horrible: Allocate arrays for meters
  double** vcaMeter = new double*[nChans];
  double** scMeter = new double* [nChans];
  double** outMeter = new double* [nChans];
  double** delayedInput = new double* [nChans];
  for (int i = 0; i < nChans; i++) {
    vcaMeter[i] = new double[nFrames];
    scMeter[i] = new double[nFrames];
    outMeter[i] = new double[nFrames];
    delayedInput[i] = new double[nFrames];
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
    // Process signal thru compressors using sidechain
    for (int i = 0; i < nChans; i++)
    {
      auto delay = &delays[i];
      for (int s = 0; s < nFrames; s++)
      {
        delay->Write(inputs[i][s]);
        delayedInput[i][s] = delay->Read();
      }
      (&compressors[i])->ProcessBlock(delayedInput[i], sidechain, outputs[i], vcaMeter[i], nFrames);

      // Apply output gain
      for (int s = 0; s < nFrames; s++)
      {
        outputs[i][s] = outputs[i][s] * linearGain;
      }
    }
  }
  else
  {
    // Process signal thru compressors using the inputs as sidechain
    for (int i = 0; i < nChans; i++)
    {
      auto delay = &delays[i];
      for (int s = 0; s < nFrames; s++)
      {
        delay->Write(inputs[i][s]);
        delayedInput[i][s] = delay->Read();
      }
      (&compressors[i])->ProcessBlock(delayedInput[i], inputs[i], outputs[i], vcaMeter[i], nFrames);

      // Apply output gain
      for (int s = 0; s < nFrames; s++)
      {
        outputs[i][s] = outputs[i][s] * linearGain;
      }
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
  for (int channel = 0; channel < nChans; channel++) {
    delete[] vcaMeter[channel];
    delete[] scMeter[channel];
    delete[] outMeter[channel];
    delete[] delayedInput[channel];
  }
  delete[] vcaMeter;
  delete[] scMeter;
  delete[] outMeter;
  delete[] delayedInput;

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


void Comprezz::UpdateDelaySamples()
{
  double sampleRate = GetSampleRate();
  double attackTimeMs = GetParam(kAttackMs)->Value();
  int delaySamples = std::ceil(sampleRate * attackTimeMs / 1000.);

  for (auto& delay : delays) delay.SetDelaySamples(delaySamples);
  SetLatency(delaySamples);
}

void Comprezz::ClearDelaySamples()
{
  for (auto& delay : delays) delay.SetDelaySamples(0);
  SetLatency(0);
}
#endif
