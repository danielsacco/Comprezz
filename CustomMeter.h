#pragma once

#include <IVMeterControl.h>

template <int MAXNC = 1>
class IVMixedMeterControl : public IVMeterControl<MAXNC>
{
public:
  IVMixedMeterControl(
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
    : IVMeterControl<MAXNC>(bounds, label, style, dir, trackNames, totalNSegs, IVMeterControl<MAXNC>::EResponse::Log, lowRangeDB, highRangeDB, markers)
  {
  }

  virtual void DrawTrackHandle(IGraphics& g, const IRECT& r, int chIdx, bool aboveBaseValue) override
  {
    auto currentValue = std::pow(GetValue(chIdx), 3);

    IColor minColor = GetColor(kX2);
    IColor maxColor = GetColor(kX3);
    IColor topColor = IColor::LinearInterpolateBetween(minColor, maxColor, currentValue);

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