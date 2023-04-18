#pragma once

class PeakDetector {
public:
  PeakDetector(double sampleRate, double attackMs, double releaseMs);
  double ProcessSample(double input);

  void setSampleRate(double sampleRate);
  void setAttack(double attackMs);
  void setRelease(double releaseMs);

private:
  void calculateConstants();

  double sampleRate;
  double attackMs;
  double releaseMs;
  double lastOutput = .0;
  double lastInput = .0;
  double alphaAttack = .9;
  double alphaRelease = .8;
};
