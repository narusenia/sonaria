#include "../Source/Audio/Synthesis/EngineMix.h"
#include "TestAssert.h"
#include <cmath>
#include <iostream>

int main()
{
  constexpr int hop = 100;
  const std::vector<float> first(400, 1.0f), second(400, 2.0f);
  const std::vector<std::uint8_t> uses = {0, 0, 1, 1};

  // Voiced boundary: equal-power crossfade of 40 samples centred on sample 200.
  auto out = engineMix::mix(first, second, uses, {1, 1, 1, 1}, hop, 40);
  CHECK(out[179] == 1.0f && out[220] == 2.0f);
  for (int i = 180; i < 220; ++i) {
    const float t = (i - 180 + 0.5f) / 40.0f;
    const float c = std::cos(t * 1.5707963f), s = std::sin(t * 1.5707963f);
    CHECK(std::abs(c * c + s * s - 1.0f) < 1e-5f);
    CHECK(std::abs(out[static_cast<size_t>(i)] - (c + 2.0f * s)) < 1e-4f);
  }

  // Unvoiced boundary: hard switch, no frame plays both engines.
  out = engineMix::mix(first, second, uses, {1, 0, 1, 1}, hop, 40);
  CHECK(out[199] == 1.0f && out[200] == 2.0f);

  // Single engine passes through; output is the shorter length.
  out = engineMix::mix(first, std::vector<float>(350, 2.0f), {1, 1, 1, 1},
                       {1, 1, 1, 1}, hop, 40);
  CHECK(out.size() == 350 && out.front() == 2.0f && out.back() == 2.0f);

  std::cout << "Engine mix tests passed\n";
}
