#include "../Source/Audio/SmartSplit.h"
#include "TestAssert.h"
#include <iostream>

using namespace smartsplit;

namespace
{
std::vector<float> tones(std::initializer_list<std::pair<float, int>> parts)
{
  std::vector<float> curve;
  for (const auto &[midi, frames] : parts)
    curve.insert(curve.end(), static_cast<size_t>(frames), midi);
  return curve;
}
} // namespace

int main()
{
  // Sensitivity 0.5 is the spec's default; the ends halve and double it.
  CHECK(std::abs(settingsFor(0.5).thresholdCents - 50.0) < 1e-9);
  CHECK(std::abs(settingsFor(0.5).stableMs - 60.0) < 1e-9);
  CHECK(settingsFor(1.0).thresholdCents < settingsFor(0.0).thresholdCents);
  CHECK(settingsFor(1.0).stableMs < settingsFor(0.0).stableMs);

  // A step of a semitone between two held tones: one split at the step.
  {
    const auto splits = findSplitPoints(tones({{60.0f, 10}, {61.0f, 10}}), 50.0, 5);
    CHECK(splits.size() == 1 && splits[0] == 10);
  }

  // A glide between the tones: the split sits in the middle of the glide.
  {
    auto curve = tones({{60.0f, 10}});
    for (int i = 1; i <= 4; ++i)
      curve.push_back(60.0f + 0.4f * i); // 60.4 .. 61.6
    const auto rest = tones({{62.0f, 10}});
    curve.insert(curve.end(), rest.begin(), rest.end());
    const auto splits = findSplitPoints(curve, 50.0, 5);
    CHECK(splits.size() == 1 && splits[0] >= 10 && splits[0] <= 14);
  }

  // Below the threshold, or a side shorter than the stable time: no split.
  CHECK(findSplitPoints(tones({{60.0f, 10}, {60.3f, 10}}), 50.0, 5).empty());
  CHECK(findSplitPoints(tones({{60.0f, 10}, {61.0f, 3}}), 50.0, 5).empty());
  // ... but a lower threshold / shorter stable time finds them.
  CHECK(findSplitPoints(tones({{60.0f, 10}, {60.3f, 10}}), 25.0, 5).size() == 1);
  CHECK(findSplitPoints(tones({{60.0f, 10}, {61.0f, 3}}), 50.0, 3).size() == 1);

  // Three tones, unvoiced frames between two of them; a return to the same
  // tone after a blip does not split.
  {
    const auto splits = findSplitPoints(
        tones({{60.0f, 8}, {0.0f, 4}, {62.0f, 8}, {64.0f, 8}}), 50.0, 5);
    CHECK(splits.size() == 2 && splits[0] == 10 && splits[1] == 20);
    CHECK(findSplitPoints(tones({{60.0f, 8}, {63.0f, 2}, {60.0f, 8}}), 50.0, 5).empty());
  }

  std::cout << "Smart split tests passed\n";
}
