#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <vector>

namespace engineMix {

/**
 * Stitch two renders of the same span frame by frame.
 *
 * usesSecond[f] picks @p second for output frame f, @p first otherwise.
 * Where the choice changes between two voiced frames, the switch is an
 * equal-power crossfade of @p fadeSamples centred on the boundary; at an
 * unvoiced boundary it is a hard switch, since the blend mask already hands
 * those frames to the original recording. Outside a crossfade exactly one
 * engine sounds.
 */
inline std::vector<float> mix(const std::vector<float> &first,
                              const std::vector<float> &second,
                              const std::vector<std::uint8_t> &usesSecond,
                              const std::vector<std::uint8_t> &voiced,
                              int hopSize, int fadeSamples)
{
  const size_t length = std::min(first.size(), second.size());
  std::vector<float> weight(length, 0.0f); // gain of `second`
  const int numFrames = static_cast<int>(usesSecond.size());
  for (size_t i = 0; i < length; ++i)
  {
    const int frame = std::min(numFrames - 1, static_cast<int>(i) / hopSize);
    weight[i] = frame >= 0 && usesSecond[static_cast<size_t>(frame)] ? 1.0f : 0.0f;
  }

  const int half = std::max(1, fadeSamples / 2);
  for (int frame = 1; frame < numFrames; ++frame)
  {
    if (usesSecond[static_cast<size_t>(frame)] == usesSecond[static_cast<size_t>(frame - 1)])
      continue;
    const bool bothVoiced = static_cast<size_t>(frame) < voiced.size() &&
                            voiced[static_cast<size_t>(frame - 1)] &&
                            voiced[static_cast<size_t>(frame)];
    if (!bothVoiced)
      continue;
    const bool toSecond = usesSecond[static_cast<size_t>(frame)] != 0;
    const int boundary = frame * hopSize;
    for (int i = boundary - half; i < boundary + half; ++i)
    {
      if (i < 0 || static_cast<size_t>(i) >= length)
        continue;
      const float t = static_cast<float>(i - (boundary - half) + 0.5f) /
                      static_cast<float>(2 * half);
      weight[static_cast<size_t>(i)] = toSecond ? t : 1.0f - t;
    }
  }

  std::vector<float> out(length);
  constexpr float halfPi = 1.5707963267948966f;
  for (size_t i = 0; i < length; ++i)
  {
    const float w = weight[i];
    out[i] = w <= 0.0f ? first[i]
           : w >= 1.0f ? second[i]
                       : first[i] * std::cos(w * halfPi) + second[i] * std::sin(w * halfPi);
  }
  return out;
}

} // namespace engineMix
