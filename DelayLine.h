#pragma once

#include <array>

const double FILL_VALUE = 0.;

template <int BUFFER_SIZE = 8192>
class DelayLine {
public:
  DelayLine() {
    buffer.fill(FILL_VALUE);
  }

  DelayLine(int samples) {
    buffer.fill(FILL_VALUE);
    SetDelaySamples(samples);
  }

  /*
  * Writes data at the end of the buffer.
  */
  void Write(double value) {
    buffer[end] = value;
    end++;
    if (end >= BUFFER_SIZE) end = 0;
  }

  void SetDelaySamples(int samples) {
    // TODO use an assertion instead of ignoring the incorrect value.
    if (samples >= BUFFER_SIZE || samples < 0) return;

    delaySamples = samples;

    start = end - samples;
    if (start < 0) start += BUFFER_SIZE;
  }

  /*
  * Reads the value from the start of the buffer.
  */
  double Read() {
    double value = buffer[start];
    start++;
    if (start >= BUFFER_SIZE) start = 0;
    return value;
  }

private:
  std::array<double, BUFFER_SIZE> buffer;
  int start = 0;
  int end = 0;
  int delaySamples = 0;
};



