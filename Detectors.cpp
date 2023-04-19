#include "Detectors.h"
#include <algorithm>
#include <cmath>

/*
* Decoupled Peak Detector implementation.
* http://c4dm.eecs.qmul.ac.uk/audioengineering/compressors/documents/report.pdf page 32
* Udo Zolzer DAFX 2nd Ed. page 230
*/
//DecoupledPeakDetector::DecoupledPeakDetector(double sampleRate, double attackMs, double releaseMs)
//  : sampleRate{sampleRate}
//{
//
//}


double DecoupledPeakDetector::ProcessSample(double input)
{
  const double x = std::abs(input);

  const double k = x > lastOutput ? alphaAttack : alphaRelease;

  lastOutput += k * (x - lastOutput);

  return lastOutput;
}




Detector::Detector(double sampleRate, double attackMs, double releaseMs)
  : sampleRate{ sampleRate }, attackMs{ attackMs }, releaseMs{ releaseMs }
{
  calculateConstants();
}

void Detector::calculateConstants()
{
  double tAttack = attackMs / 1000.;
  double tRelease = releaseMs / 1000.;

  alphaAttack = 1. - std::exp(-2.2 / (tAttack * sampleRate));     // Rise Time 10%/90%
  alphaRelease = 1. - std::exp(-2.2 / (tRelease * sampleRate));   // Release Time 90%/10%

}

void Detector::setSampleRate(double sampleRate)
{
  if (sampleRate != Detector::sampleRate)
  {
    Detector::sampleRate = sampleRate;
    calculateConstants();
  }
}

void Detector::setAttack(double attackMs)
{
  if (attackMs != Detector::attackMs)
  {
    Detector::attackMs = attackMs;
    calculateConstants();
  }
}

void Detector::setRelease(double releaseMs)
{
  if (releaseMs != Detector::releaseMs)
  {
    Detector::releaseMs = releaseMs;
    calculateConstants();
  }
}
