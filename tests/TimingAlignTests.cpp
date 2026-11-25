#include "../Source/Audio/TimingAlign.h"
#include "TestAssert.h"
#include <iostream>

using namespace timingalign;

int main()
{
  // Matching: the reference with the largest overlap; none without overlap.
  const std::vector<Span> references{{0.0, 1.0}, {1.0, 2.5}, {3.0, 4.0}};
  CHECK(bestReference({0.8, 2.0}, references) == 1);
  CHECK(bestReference({0.0, 0.6}, references) == 0);
  CHECK(bestReference({2.6, 2.9}, references) == -1);
  CHECK(bestReference({2.5, 3.0}, references) == -1); // touching is no overlap
  CHECK(bestReference({0.0, 1.0}, {}) == -1);

  // Strength scales the move of both boundaries.
  const auto half = aligned({1.0, 2.0}, {1.2, 1.8}, 0.5);
  CHECK(std::abs(half.start - 1.1) < 1e-9 && std::abs(half.end - 1.9) < 1e-9);
  const auto full = aligned({1.0, 2.0}, {1.2, 1.8}, 1.0);
  CHECK(full.start == 1.2 && full.end == 1.8);
  const auto none = aligned({1.0, 2.0}, {1.2, 1.8}, 0.0);
  CHECK(none.start == 1.0 && none.end == 2.0);

  // A lone note takes its target.
  {
    const auto moved = moveBoundaries({{10, 20, 0, 100}}, {FrameSpan{12, 25}});
    CHECK(moved[0].start == 12 && moved[0].end == 25);
  }

  // A shared boundary moves the unselected neighbour with it.
  {
    const auto moved = moveBoundaries({{0, 10, 0, 100}, {10, 20, 0, 100}, {20, 30, 0, 100}},
                                      {std::nullopt, FrameSpan{8, 23}, std::nullopt});
    CHECK(moved[0].start == 0 && moved[0].end == 8);
    CHECK(moved[1].start == 8 && moved[1].end == 23);
    CHECK(moved[2].start == 23 && moved[2].end == 30);
  }

  // A gap keeps one frame; a neighbour is never inverted.
  {
    const auto moved = moveBoundaries({{0, 10, 0, 100}, {15, 20, 0, 100}},
                                      {std::nullopt, FrameSpan{5, 20}});
    CHECK(moved[0].end == 10 && moved[1].start == 11);
    const auto squeezed = moveBoundaries({{0, 10, 0, 100}, {10, 20, 0, 100}},
                                         {std::nullopt, FrameSpan{-5, 20}});
    CHECK(squeezed[0].end == 1 && squeezed[1].start == 1);
  }

  // Region lead and tail limits, and a note keeps at least one frame.
  {
    const auto moved = moveBoundaries({{10, 20, 4, 90}}, {FrameSpan{0, 99}});
    CHECK(moved[0].start == 4 && moved[0].end == 90);
    const auto collapsed = moveBoundaries({{10, 20, 0, 100}}, {FrameSpan{18, 12}});
    CHECK(collapsed[0].start == 18 && collapsed[0].end == 19);
  }

  // Two adjacent selected notes: the later one decides their shared boundary.
  {
    const auto moved = moveBoundaries({{0, 10, 0, 100}, {10, 20, 0, 100}},
                                      {FrameSpan{0, 12}, FrameSpan{11, 20}});
    CHECK(moved[0].end == 11 && moved[1].start == 11);
  }

  std::cout << "Timing align tests passed\n";
}
