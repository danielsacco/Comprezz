#pragma once

class Detector {
public:
  Detector(double sampleRate, double attackMs, double releaseMs);

  virtual double ProcessSample(double input) = 0;

  void setSampleRate(double sampleRate) ;
  void setAttack(double attackMs);
  void setRelease(double releaseMs);
  //virtual ~Detector() {};

protected:
  void calculateConstants();

  double sampleRate;
  double attackMs;
  double releaseMs;
  double alphaAttack = .9;
  double alphaRelease = .8;

};

/*
* Decoupled Peak Detector.
*/
class DecoupledPeakDetector : virtual public Detector {

public:
  using Detector::Detector;
  DecoupledPeakDetector(double sampleRate, double attackMs, double releaseMs) : Detector(sampleRate, attackMs, releaseMs) {};
  virtual double ProcessSample(double input) override;

private:
  double lastOutput = .0;
};


