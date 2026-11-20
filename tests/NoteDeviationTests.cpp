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

  std::cout << "Note deviation tests passed\n";
}
