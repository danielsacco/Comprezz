#pragma once

#include "IVMeterControl.h"

//using namespace iplug;
//using namespace igraphics;

BEGIN_IPLUG_NAMESPACE
BEGIN_IGRAPHICS_NAMESPACE

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
    IVMeterControl<>::EResponse response = IVMeterControl<>::EResponse::Linear,
    float lowRangeDB = -72.f,
    float highRangeDB = 12.f,
    std::initializer_list<int> markers = { 0, -6, -12, -24, -48 })
    : IVMeterControl<MAXNC>{
        bounds,
        label,
        style,
        dir,
        trackNames,
        totalNSegs
    }
    //: IVMeterControl<MAXNC>(bounds, label, style, dir, trackNames, totalNSegs, response, lowRangeDB, highRangeDB, markers)
  {
  };

  virtual void DrawTrackHandle(IGraphics& g, const IRECT& r, int chIdx, bool aboveBaseValue) override
  {
    auto interpolationValue = std::min(std::pow(IControl::GetValue(chIdx), 3), 1.);

    IColor minColor = IVectorBase::GetColor(kX2);
    IColor maxColor = IVectorBase::GetColor(kX3);
    IColor topColor = IColor::LinearInterpolateBetween(minColor, maxColor, interpolationValue);

    g.FillRectWithPattern(
      IPattern::CreateLinearGradient(r, EDirection::Vertical, {
        {topColor, 0.f}, {minColor, 1.f} }
    ), r, &(IControl::mBlend));


    if (chIdx == IVTrackControlBase::mMouseOverTrack)
    {
      g.FillRect(IVectorBase::GetColor(kHL), r, &(IControl::mBlend));
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
    IVMeterControl<>::EResponse response = IVMeterControl<>::EResponse::Linear,
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

    auto interpolationValue = std::min(std::pow(IControl::GetValue(chIdx), 3), 1.);

    IColor minColor = IVectorBase::GetColor(kX2);
    IColor maxColor = IVectorBase::GetColor(kX3);
    IColor topColor = IColor::LinearInterpolateBetween(maxColor, minColor, interpolationValue);

    g.FillRectWithPattern(
      IPattern::CreateLinearGradient(invertedTrackHandle, EDirection::Vertical, {
        {minColor, 0.f}, {topColor, 1.f} }
    ), invertedTrackHandle, &(IControl::mBlend));


    if (chIdx == IVTrackControlBase::mMouseOverTrack)
    {
      g.FillRect(IVectorBase::GetColor(kHL), r, &(IControl::mBlend));
    }
  }

};

END_IGRAPHICS_NAMESPACE
END_IPLUG_NAMESPACE
