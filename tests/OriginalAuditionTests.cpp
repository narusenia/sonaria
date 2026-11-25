#include "../Source/Audio/OriginalAudition.h"
#include "TestAssert.h"
#include <cmath>
#include <iostream>

using namespace originalaudition;

int main()
{
  constexpr int fade = 441; // 10 ms at 44.1 kHz
  // Render = +0.5, original = -0.5 over [1000, 5000): the worst case for a click.
  std::vector<Slice> slices{{1000, std::vector<float>(4000, -0.5f)}};

  // Held from the start: outside the slice the render is untouched, inside
  // it reaches the original, and every step between samples stays small.
  {
    std::vector<float> out(6000, 0.5f);
    float mix = 0.0f;
    blend(out.data(), static_cast<int>(out.size()), 0.0, 1.0, slices, true, mix, fade);
    CHECK(out[500] == 0.5f && out[5500] == 0.5f);
    CHECK(out[3000] == -0.5f);
    for (size_t i = 1; i < out.size(); ++i)
      CHECK(std::abs(out[i] - out[i - 1]) <= 1.0f / fade + 1e-6f);
  }

  // Pressed and released mid-slice, across blocks: the mix ramps both ways.
  {
    std::vector<float> out(4000, 0.5f);
    float mix = 0.0f;
    blend(out.data(), 2000, 1000.0, 1.0, slices, true, mix, fade);
    CHECK(mix == 1.0f);
    blend(out.data() + 2000, 2000, 3000.0, 1.0, slices, false, mix, fade);
    CHECK(mix == 0.0f && out[3999] == 0.5f);
    for (size_t i = 1; i < out.size(); ++i)
      CHECK(std::abs(out[i] - out[i - 1]) <= 2.0f / fade + 1e-6f);
  }

  // Not held and fully faded: nothing changes.
  {
    std::vector<float> out(100, 0.25f);
    float mix = 0.0f;
    blend(out.data(), 100, 2000.0, 1.0, slices, false, mix, fade);
    CHECK(out[50] == 0.25f);
  }

  std::cout << "Original audition tests passed\n";
}
