#pragma once

#include "../../JuceHeader.h"
#include "../../Models/Project.h"
#include "../../Utils/ScaleUtils.h"
#include "../../Utils/UI/Theme.h"

namespace pianoRollView
{
inline juce::Colour getScaleAccentColour(ScaleMode mode)
{
  switch (mode)
  {
  case ScaleMode::None:
    return APP_COLOR_PRIMARY;
  case ScaleMode::Major:
    return APP_COLOR_SCALE_MAJOR;
  case ScaleMode::Minor:
    return APP_COLOR_SCALE_MINOR;
  case ScaleMode::Dorian:
    return APP_COLOR_SCALE_DORIAN;
  case ScaleMode::Phrygian:
    return APP_COLOR_SCALE_PHRYGIAN;
  case ScaleMode::Lydian:
    return APP_COLOR_SCALE_LYDIAN;
  case ScaleMode::Mixolydian:
    return APP_COLOR_SCALE_MIXOLYDIAN;
  case ScaleMode::Locrian:
    return APP_COLOR_SCALE_LOCRIAN;
  case ScaleMode::Blues:
    return APP_COLOR_SCALE_BLUES;
  case ScaleMode::HarmonicMinor:
    return APP_COLOR_SCALE_HARMONIC_MINOR;
  case ScaleMode::MajorPentatonic:
    return APP_COLOR_SCALE_MAJOR_PENTATONIC;
  case ScaleMode::MelodicMinor:
    return APP_COLOR_SCALE_MELODIC_MINOR;
  case ScaleMode::MinorPentatonic:
    return APP_COLOR_SCALE_MINOR_PENTATONIC;
  case ScaleMode::PhrygianDominant:
    return APP_COLOR_SCALE_PHRYGIAN_DOMINANT;
  case ScaleMode::WholeTone:
    return APP_COLOR_SCALE_WHOLE_TONE;
  case ScaleMode::Chromatic:
    return APP_COLOR_PRIMARY;
  }
  return APP_COLOR_PRIMARY;
}

inline bool isBlackKey(int noteInOctave)
{
  return noteInOctave == 1 || noteInOctave == 3 || noteInOctave == 6 ||
         noteInOctave == 8 || noteInOctave == 10;
}

enum class ScaleToneState
{
  Root,
  InScale,
  OutOfScale
};

inline ScaleToneState getScaleToneState(ScaleMode mode, int noteInOctave, int rootNote)
{
  if (mode == ScaleMode::None || rootNote < 0)
    return ScaleToneState::InScale;

  const int normalizedNote = (noteInOctave % 12 + 12) % 12;
  const int normalizedRoot = (rootNote % 12 + 12) % 12;

  if (mode == ScaleMode::Chromatic)
    return ScaleToneState::InScale;
  if (normalizedNote == normalizedRoot)
    return ScaleToneState::Root;
  if (ScaleUtils::isPitchClassInScale(mode, normalizedNote, normalizedRoot))
    return ScaleToneState::InScale;
  return ScaleToneState::OutOfScale;
}

inline bool isMultipleOf(double value, double step)
{
  if (step <= 0.0)
    return false;
  const double ratio = value / step;
  return std::abs(ratio - std::round(ratio)) < 1.0e-4;
}
} // namespace pianoRollView
