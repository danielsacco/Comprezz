#pragma once

#include "IVMeterControl.h"

using namespace iplug;
using namespace igraphics;

template <int MAXNC = 1>
class IVPatternMeterControl : public IVMeterControl<MAXNC>
{
public:
  IVPatternMeterControl(
    const IRECT& bounds,
    const char* label,
    const IVStyle& style = DEFAULT_STYLE,
    EDirection dir = EDirection::Vertical,
    std::initializer_list<const char*> trackNames = {},
    int totalNSegs = 0,
    EResponse response = EResponse::Linear,
    float lowRangeDB = -72.f,
    float highRangeDB = 12.f,
    std::initializer_list<int> markers = { 0, -6, -12, -24, -48 })
    : IVMeterControl<MAXNC>(bounds, label, style, dir, trackNames, totalNSegs, response, lowRangeDB, highRangeDB, markers)
  {
  };

  virtual void DrawTrackHandle(IGraphics& g, const IRECT& r, int chIdx, bool aboveBaseValue) override
  {
    auto interpolationValue = std::min(std::pow(GetValue(chIdx), 3), 1.);

    IColor minColor = GetColor(kX2);
    IColor maxColor = GetColor(kX3);
    IColor topColor = IColor::LinearInterpolateBetween(minColor, maxColor, interpolationValue);

    g.FillRectWithPattern(
      IPattern::CreateLinearGradient(r, EDirection::Vertical, {
        {topColor, 0.f}, {minColor, 1.f} }
    ), r, &mBlend);


    if (chIdx == mMouseOverTrack)
    {
      g.FillRect(GetColor(kHL), r, &mBlend);
    }
  }

};

template <int MAXNC = 1>
class IVInvertedPatternMeterControl : public IVPatternMeterControl<MAXNC>
{
public:
  IVInvertedPatternMeterControl(
    const IRECT& bounds,
    const char* label,
    const IVStyle& style = DEFAULT_STYLE,
    EDirection dir = EDirection::Vertical,
    std::initializer_list<const char*> trackNames = {},
    int totalNSegs = 0,
    EResponse response = EResponse::Linear,
    float lowRangeDB = -72.f,
    float highRangeDB = 12.f,
    std::initializer_list<int> markers = { 0, -6, -12, -24, -48 })
    : IVPatternMeterControl<MAXNC>(bounds, label, style, dir, trackNames, totalNSegs, response, lowRangeDB, highRangeDB, markers)
  {
  };

  virtual void DrawTrackHandle(IGraphics& g, const IRECT& r, int chIdx, bool aboveBaseValue) override
  {
    // Ineverted track should preserve L & R but:
    // - bottom should be r.top
    // - top should be mTargetRect.top
    IRECT invertedTrackHandle = IRECT(r.L, IControl::mTargetRECT.T, r.R, r.T);

    auto interpolationValue = std::min(std::pow(GetValue(chIdx), 3), 1.);

    IColor minColor = GetColor(kX2);
    IColor maxColor = GetColor(kX3);
    IColor topColor = IColor::LinearInterpolateBetween(maxColor, minColor, interpolationValue);

    g.FillRectWithPattern(
      IPattern::CreateLinearGradient(invertedTrackHandle, EDirection::Vertical, {
        {minColor, 0.f}, {topColor, 1.f} }
    ), invertedTrackHandle, &mBlend);


    if (chIdx == mMouseOverTrack)
    {
      g.FillRect(GetColor(kHL), r, &mBlend);
    }
  }

};