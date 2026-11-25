#pragma once

#include <algorithm>
#include <cstdint>
#include <vector>

/**
 * A/B audition (REQ-WF-006): while held, the selected notes play the original
 * recording instead of the corrected render. Both the switch and the slice
 * edges fade over `fadeSamples` (about 10 ms), so neither clicks.
 */
namespace originalaudition
{
struct Slice
{
  int64_t start = 0;          // waveform sample where `samples` begins
  std::vector<float> samples; // the original recording over the slice
};

/**
 * Blends `output` (the render, n samples whose first one reads waveform
 * position `startPosition`, advancing `ratio` per sample) toward the
 * slices. `mix` (0 render .. 1 original) moves toward `on` by 1/fadeSamples
 * per sample and is kept across blocks by the caller.
 */
inline void blend(float *output, int n, double startPosition, double ratio,
                  const std::vector<Slice> &slices, bool on, float &mix,
                  int fadeSamples)
{
  const float step = 1.0f / static_cast<float>(std::max(1, fadeSamples));
  const float target = on ? 1.0f : 0.0f;
  size_t hint = 0;
  for (int i = 0; i < n; ++i)
  {
    mix = mix < target ? std::min(target, mix + step) : std::max(target, mix - step);
    if (mix <= 0.0f)
      continue;
    const auto position = static_cast<int64_t>(startPosition + i * ratio);
    for (size_t k = 0; k < slices.size(); ++k)
    {
      const auto &slice = slices[(hint + k) % slices.size()];
      const int64_t offset = position - slice.start;
      const auto length = static_cast<int64_t>(slice.samples.size());
      if (offset < 0 || offset >= length)
        continue;
      hint = (hint + k) % slices.size();
      const float edge = std::clamp(
          static_cast<float>(std::min(offset, length - 1 - offset)) * step, 0.0f, 1.0f);
      const float weight = mix * edge;
      output[i] += weight * (slice.samples[static_cast<size_t>(offset)] - output[i]);
      break;
    }
  }
}
} // namespace originalaudition
