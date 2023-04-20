#pragma once

#include <IVMeterControl.h>

template <int MAXNC = 1>
class IVInOutMeterControl : public IVMeterControl<MAXNC>
{
public:
  IVInOutMeterControl(
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

};