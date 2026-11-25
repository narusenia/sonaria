#pragma once

#include <algorithm>
#include <cstddef>
#include <optional>
#include <vector>

// Select All in ARA mode: the first press selects one playback region's notes,
// a second press (that region already exactly selected) selects every note.
namespace regionselect
{
// One playback region of the active Project, in host-timeline seconds.
// offset is the host time of the Project's zero, so a note at project time t
// plays at t + offset.
struct RegionSpan
{
  double start = 0.0, end = 0.0, offset = 0.0;
  bool containsProjectTime(double t) const { return t + offset >= start && t + offset < end; }
  bool containsHostTime(double t) const { return t >= start && t < end; }
};

// noteStarts: project-time start of each selectable note (rests excluded).
// selected: whether each of those notes is selected now.
// playhead: host-timeline seconds, if known.
// Returns the indices of the notes to select.
inline std::vector<size_t> pickNotes(const std::vector<double> &noteStarts,
                                     const std::vector<bool> &selected,
                                     const std::vector<RegionSpan> &regions,
                                     std::optional<double> playhead)
{
  std::vector<size_t> all(noteStarts.size());
  for (size_t i = 0; i < all.size(); ++i)
    all[i] = i;

  // A note belongs to the region holding its start; the first selected note
  // picks the region, else the playhead does.
  const RegionSpan *region = nullptr;
  const auto firstSelected = std::find(selected.begin(), selected.end(), true);
  if (firstSelected != selected.end())
  {
    const double start = noteStarts[static_cast<size_t>(firstSelected - selected.begin())];
    for (const auto &r : regions)
      if (r.containsProjectTime(start)) { region = &r; break; }
  }
  if (region == nullptr && playhead)
    for (const auto &r : regions)
      if (r.containsHostTime(*playhead)) { region = &r; break; }
  if (region == nullptr)
    return all;

  std::vector<size_t> inRegion;
  bool exactlySelected = true;
  for (size_t i = 0; i < noteStarts.size(); ++i)
  {
    const bool inside = region->containsProjectTime(noteStarts[i]);
    if (inside)
      inRegion.push_back(i);
    exactlySelected = exactlySelected && inside == selected[i];
  }
  return inRegion.empty() || exactlySelected ? all : inRegion;
}
}
