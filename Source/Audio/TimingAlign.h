#pragma once

#include <algorithm>
#include <cmath>
#include <optional>
#include <vector>

/**
 * Timing align (REQ-HARM-003): moves notes' starts and ends toward the notes
 * of a reference part (the pinned tracks). Pure, so it is unit tested; the
 * piano roll converts times and commits through the timing tool's path.
 */
namespace timingalign
{
struct Span
{
  double start = 0.0;
  double end = 0.0;
};

/** The reference with the largest time overlap with `own`, or -1 for none. */
inline int bestReference(Span own, const std::vector<Span> &references)
{
  int best = -1;
  double bestOverlap = 0.0;
  for (size_t i = 0; i < references.size(); ++i)
  {
    const double overlap = std::min(own.end, references[i].end) -
                           std::max(own.start, references[i].start);
    if (overlap > bestOverlap)
    {
      bestOverlap = overlap;
      best = static_cast<int>(i);
    }
  }
  return best;
}

/** Start and end moved by strength (0..1) x (reference - own). */
inline Span aligned(Span own, Span reference, double strength)
{
  strength = std::clamp(strength, 0.0, 1.0);
  return {own.start + strength * (reference.start - own.start),
          own.end + strength * (reference.end - own.end)};
}

struct FrameSpan
{
  int start = 0;
  int end = 0;
  // Earliest start / latest end the note may take (a region's unpitched lead
  // and tail move with its first and last note and must stay in the audio).
  int minStart = 0;
  int maxEnd = 0;
};

/**
 * Commits targets onto notes sorted by start, as the timing tool would drag
 * their boundaries: a boundary shared with a neighbour moves both notes, a
 * gap to a neighbour keeps at least one frame, and every note keeps at least
 * one frame. Notes without a target only move where they share a boundary.
 */
inline std::vector<FrameSpan>
moveBoundaries(std::vector<FrameSpan> notes,
               const std::vector<std::optional<FrameSpan>> &targets)
{
  const int count = static_cast<int>(notes.size());
  for (int i = 0; i < count; ++i)
  {
    const auto &target = targets[static_cast<size_t>(i)];
    if (!target)
      continue;
    auto &note = notes[static_cast<size_t>(i)];
    FrameSpan *previous = i > 0 ? &notes[static_cast<size_t>(i - 1)] : nullptr;
    FrameSpan *next = i + 1 < count ? &notes[static_cast<size_t>(i + 1)] : nullptr;

    const bool sharesStart = previous && previous->end == note.start;
    int lowest = note.minStart;
    if (previous)
      lowest = std::max(lowest, sharesStart ? previous->start + 1 : previous->end + 1);
    const int start = std::clamp(target->start, lowest, std::max(lowest, note.end - 1));
    if (sharesStart)
      previous->end = start;
    note.start = start;

    const bool sharesEnd = next && next->start == note.end;
    int highest = note.maxEnd;
    if (next)
      highest = std::min(highest, sharesEnd ? next->end - 1 : next->start - 1);
    const int end = std::clamp(target->end, std::min(note.start + 1, highest), highest);
    if (sharesEnd)
      next->start = end;
    note.end = end;
  }
  return notes;
}
} // namespace timingalign
