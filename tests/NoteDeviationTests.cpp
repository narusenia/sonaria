#include "../Source/Utils/NoteDeviation.h"
#include "TestAssert.h"
#include <cmath>
#include <iostream>

static bool near(float a, float b) { return std::abs(a - b) < 0.05f; }

int main()
{
  Project project;
  project.setProjectKey(0, ScaleMode::Major);
  project.setScaleMode(ScaleMode::Major);
  auto& audio = project.getAudioData();
  audio.f0.assign(40, 0.0f);
  audio.voicedMask.assign(40, false);

  // Voiced frames at E4 +20 ct and +40 ct average to +30 ct; unvoiced frames are ignored.
  Note note(0, 10, 64.0f);
  for (int i = 0; i < 8; ++i) {
    audio.f0[static_cast<size_t>(i)] = midiToFreq(i < 4 ? 64.2f : 64.4f);
    audio.voicedMask[static_cast<size_t>(i)] = true;
  }
  audio.f0[8] = midiToFreq(70.0f);
  CHECK(near(NoteDeviation::cents(project, note), 30.0f));

  // F#4 -10 ct is not in C major: the nearest scale tone is F4, 90 ct below.
  Note outOfScale(10, 20, 66.0f);
  for (int i = 10; i < 20; ++i) {
    audio.f0[static_cast<size_t>(i)] = midiToFreq(65.9f);
    audio.voicedMask[static_cast<size_t>(i)] = true;
  }
  CHECK(near(NoteDeviation::cents(project, outOfScale), 90.0f));

  // In chromatic mode the nearest semitone is the target.
  project.setScaleMode(ScaleMode::Chromatic);
  CHECK(near(NoteDeviation::cents(project, outOfScale), -10.0f));

  // A note with no voiced frames falls back to its own pitch.
  Note silent(30, 40, 60.0f);
  silent.setPitchOffset(0.25f);
  CHECK(near(NoteDeviation::cents(project, silent), 25.0f));

  // Default thresholds: under 10 ct in tune, 10 to under 30 caution, 30 and over fix.
  CHECK(NoteDeviation::level(9.9f) == NoteDeviation::Level::InTune);
  CHECK(NoteDeviation::level(-10.0f) == NoteDeviation::Level::Caution);
  CHECK(NoteDeviation::level(29.9f) == NoteDeviation::Level::Caution);
  CHECK(NoteDeviation::level(-30.0f) == NoteDeviation::Level::Fix);
  CHECK(NoteDeviation::level(20.0f, {5.0f, 15.0f}) == NoteDeviation::Level::Fix);

  // Jumps visit only notes needing a fix, in time order, and wrap at either end.
  {
    Project jumps;
    jumps.setScaleMode(ScaleMode::Chromatic);
    const float pitches[] = {60.0f, 60.4f, 62.0f, 63.35f, 65.0f};
    for (int i = 0; i < 5; ++i) {
      jumps.addNote(Note(i * 10, i * 10 + 10, 60.0f));
      jumps.getNotes().back().setPitchOffset(pitches[i] - 60.0f);
    }
    CHECK(NoteDeviation::findFixNote(jumps, -1, true) == 1);
    CHECK(NoteDeviation::findFixNote(jumps, 10, true) == 3);
    CHECK(NoteDeviation::findFixNote(jumps, 30, true) == 1);
    CHECK(NoteDeviation::findFixNote(jumps, 30, false) == 1);
    CHECK(NoteDeviation::findFixNote(jumps, 10, false) == 3);
    // Including caution notes adds the 15 ct note (index 2 at 62.15) to the walk.
    jumps.getNotes()[2].setPitchOffset(2.15f);
    CHECK(NoteDeviation::findFixNote(jumps, 10, true, true) == 2);
    CHECK(NoteDeviation::findFixNote(jumps, 10, true, false) == 3);
    Project clean;
    clean.addNote(Note(0, 10, 60.0f));
    CHECK(NoteDeviation::findFixNote(clean, -1, true) == -1);
  }

  std::cout << "Note deviation tests passed\n";
}
