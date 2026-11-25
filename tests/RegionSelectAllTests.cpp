// Select All picks one ARA playback region first, then every note.
#include "../Source/Models/RegionSelectAll.h"
#include "TestAssert.h"
#include <iostream>

using regionselect::pickNotes;
using regionselect::RegionSpan;
using Indices = std::vector<size_t>;

int main()
{
  // Notes at project time 0, 1, 2 | 5, 6. Region A plays project 0..4 at host
  // 10..14, region B plays project 4..8 at host 20..24.
  const std::vector<double> starts{0.0, 1.0, 2.0, 5.0, 6.0};
  const std::vector<RegionSpan> regions{{10.0, 14.0, 10.0}, {20.0, 24.0, 16.0}};
  const Indices all{0, 1, 2, 3, 4};
  const std::vector<bool> none(5, false);

  // No regions (standalone): everything, as before.
  CHECK(pickNotes(starts, none, {}, 21.0) == all);
  // Nothing selected: the region under the playhead.
  CHECK(pickNotes(starts, none, regions, 21.0) == (Indices{3, 4}));
  CHECK(pickNotes(starts, none, regions, 11.0) == (Indices{0, 1, 2}));
  // Playhead outside every region, or unknown: everything.
  CHECK(pickNotes(starts, none, regions, 17.0) == all);
  CHECK(pickNotes(starts, none, regions, std::nullopt) == all);
  // A selection picks its region, over the playhead.
  CHECK(pickNotes(starts, {false, true, false, false, false}, regions, 21.0) ==
        (Indices{0, 1, 2}));
  // A selection across regions uses the first selected note's region.
  CHECK(pickNotes(starts, {false, false, true, true, false}, regions, 11.0) ==
        (Indices{0, 1, 2}));
  // The region already exactly selected: second press selects everything.
  CHECK(pickNotes(starts, {false, false, false, true, true}, regions, 11.0) == all);
  // A region without notes falls back to everything.
  CHECK(pickNotes(starts, none, {{30.0, 34.0, 10.0}}, 31.0) == all);

  std::cout << "Region select all tests passed\n";
}
