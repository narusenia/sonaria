// REQ-UI-002: a 3-minute, ~600-note take must scroll and zoom at 60 fps.
// Paints the piano roll offscreen frame by frame and checks the mean paint time.
#include "../Source/UI/PianoRollComponent.h"
#include "../Source/Utils/PitchCurveProcessor.h"
#include "TestAssert.h"
#include <cmath>
#include <iostream>

int main()
{
  juce::ScopedJuceInitialiser_GUI gui;

  Project project;
  auto &audio = project.getAudioData();
  audio.sampleRate = SAMPLE_RATE;
  constexpr double seconds = 180.0;
  const int frames = static_cast<int>(seconds * SAMPLE_RATE / HOP_SIZE);
  audio.waveform.setSize(1, static_cast<int>(seconds * SAMPLE_RATE));
  for (int i = 0; i < audio.waveform.getNumSamples(); ++i)
    audio.waveform.setSample(0, i, 0.3f * std::sin(i * 0.05f));
  audio.f0.assign(static_cast<size_t>(frames), 0.0f);
  audio.voicedMask.assign(static_cast<size_t>(frames), false);
  audio.basePitch.assign(static_cast<size_t>(frames), 0.0f);
  audio.deltaPitch.assign(static_cast<size_t>(frames), 0.0f);

  // 600 notes of 0.25 s with vibrato, separated by short gaps.
  const int noteFrames = frames / 600;
  for (int n = 0; n < 600; ++n)
  {
    const int start = n * noteFrames, end = start + noteFrames * 4 / 5;
    const float midi = 57.0f + static_cast<float>((n * 7) % 12) + 0.2f * static_cast<float>(n % 3);
    project.addNote(Note(start, end, midi));
    for (int f = start; f < end; ++f)
    {
      const float wobble = 0.3f * std::sin(static_cast<float>(f) * 0.6f);
      audio.f0[static_cast<size_t>(f)] = midiToFreq(midi + wobble);
      audio.voicedMask[static_cast<size_t>(f)] = true;
    }
  }
  PitchCurveProcessor::rebuildCurvesFromSource(project, audio.f0);

  PianoRollComponent roll;
  roll.setProject(&project);
  roll.setSize(1600, 900);
  juce::Image image(juce::Image::ARGB, 1600, 900, true);

  const auto paintFrames = [&](const char *label, const std::function<void(int)> &step)
  {
    constexpr int count = 120;
    const double begin = juce::Time::getMillisecondCounterHiRes();
    for (int i = 0; i < count; ++i)
    {
      step(i);
      juce::Graphics g(image);
      roll.paintEntireComponent(g, false);
    }
    const double mean = (juce::Time::getMillisecondCounterHiRes() - begin) / count;
    std::cout << label << ": " << mean << " ms per frame\n";
    return mean;
  };

  const double scroll = paintFrames("scroll", [&](int i) { roll.setScrollX(i * 40.0); });
  const double zoom = paintFrames("zoom", [&](int i)
  { roll.setPixelsPerSecond(60.0f + static_cast<float>(i % 60) * 4.0f, false); });

  // 16.7 ms is one frame at 60 fps.
  CHECK(scroll < 16.7);
  CHECK(zoom < 16.7);
  std::cout << "Piano roll frame time tests passed\n";
}
