#include "Detector.h"
#include <algorithm>
#include <cmath>

/*
* Decoupled Peak Detector.
* http://c4dm.eecs.qmul.ac.uk/audioengineering/compressors/documents/report.pdf page 32
* Udo Zolzer DAFX 2nd Ed. page 230
*/
PeakDetector::PeakDetector(double sampleRate, double attackMs, double releaseMs)
  : sampleRate{sampleRate}, attackMs{attackMs}, releaseMs{releaseMs}
{
  calculateConstants();
}

void PeakDetector::calculateConstants()
{
  double tAttack = attackMs / 1000.;
  double tRelease = releaseMs / 1000.;

  alphaAttack = 1. - std::exp(-2.2 / (tAttack * sampleRate));
  alphaRelease = 1. - std::exp(-2.2 / (tRelease * sampleRate));

}

double PeakDetector::ProcessSample(double input)
{
  const double x = std::abs(input);

  const double k = x > lastOutput ? alphaAttack : alphaRelease;

  lastOutput += k * (x - lastOutput);

  return lastOutput;
}

void PeakDetector::setSampleRate(double sampleRate)
{
  if (sampleRate != PeakDetector::sampleRate)
  {
    PeakDetector::sampleRate = sampleRate;
    calculateConstants();
  }
}

void PeakDetector::setAttack(double attackMs)
{
  PeakDetector::attackMs = attackMs;
  calculateConstants();
}

void PeakDetector::setRelease(double releaseMs)
{
  PeakDetector::releaseMs = releaseMs;
  calculateConstants();
}
