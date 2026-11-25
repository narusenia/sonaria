#pragma once

#include <cmath>
#include <vector>

/**
 * Smart split (REQ-WF-008): where a note's pitch changes from one held tone
 * to another. Pure, so it is unit tested; the piano roll previews the points
 * and splits through NoteSplitter. Never run at analysis time (ADR-0008).
 */
namespace smartsplit
{
struct Settings
{
  double thresholdCents = 50.0;
  double stableMs = 60.0;
};

/**
 * One sensitivity (0..1) drives both values: 0.5 gives the defaults
 * (50 ct, 60 ms), 0 doubles them (fewer splits), 1 halves them (more).
 */
inline Settings settingsFor(double sensitivity)
{
  const double scale = std::pow(2.0, 1.0 - 2.0 * sensitivity);
  return {50.0 * scale, 60.0 * scale};
}

/**
 * Split offsets into `midi` (one value per frame, non-finite or <= 0 for
 * unvoiced frames). Frames that stay within half the threshold of their
 * run's mean form a run; a run of at least `stableFrames` is a held tone.
 * Between two held tones whose means differ by the threshold or more, the
 * split lands in the middle of the transition. Offsets are in 1..size-1.
 */
inline std::vector<int> findSplitPoints(const std::vector<float> &midi,
                                        double thresholdCents, int stableFrames)
{
  struct Run
  {
    int start;
    int end;
    double mean;
  };
  std::vector<Run> held;
  const double band = thresholdCents / 200.0; // half the threshold, in semitones
  const int size = static_cast<int>(midi.size());
  int start = -1;
  double sum = 0.0;
  const auto close = [&](int end)
  {
    if (start >= 0 && end - start >= stableFrames)
      held.push_back({start, end, sum / (end - start)});
    start = -1;
  };
  for (int i = 0; i < size; ++i)
  {
    const float value = midi[static_cast<size_t>(i)];
    if (!std::isfinite(value) || value <= 0.0f)
    {
      close(i);
      continue;
    }
    if (start >= 0 && std::abs(value - sum / (i - start)) <= band)
    {
      sum += value;
      continue;
    }
    close(i);
    start = i;
    sum = value;
  }
  close(size);

  std::vector<int> splits;
  for (size_t k = 1; k < held.size(); ++k)
    if (std::abs(held[k].mean - held[k - 1].mean) * 100.0 >= thresholdCents)
      splits.push_back((held[k - 1].end + held[k].start) / 2);
  return splits;
}
} // namespace smartsplit
