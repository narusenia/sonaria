#pragma once

#include "../JuceHeader.h"
#include "../Models/Note.h"
#include "ScaleUtils.h"
#include <cmath>
#include <vector>

/**
 * Correction presets (REQ-WF-007): the Pitch Center popup's settings plus a
 * note engine, saved by name in the app settings and applied to the
 * selection in one step.
 */
struct CorrectionPreset
{
  juce::String name;
  float pitchCenter = 100.0f; // Quantize amount, 0..100 %
  float pitchDrift = 100.0f;  // Straighten amount, 0..100 % (100 = flat)
  bool snapToScale = false;
  bool keepVibrato = true;
  NoteEngine engine = NoteEngine::Default;

  juce::var toVar() const
  {
    auto object = std::make_unique<juce::DynamicObject>();
    object->setProperty("name", name);
    object->setProperty("pitchCenter", pitchCenter);
    object->setProperty("pitchDrift", pitchDrift);
    object->setProperty("snapToScale", snapToScale);
    object->setProperty("keepVibrato", keepVibrato);
    object->setProperty("engine", static_cast<int>(engine));
    return juce::var(object.release());
  }

  static std::vector<CorrectionPreset> listFromVar(const juce::var &list)
  {
    std::vector<CorrectionPreset> presets;
    if (const auto *array = list.getArray())
      for (const auto &item : *array)
      {
        const auto name = item.getProperty("name", {}).toString().trim();
        if (name.isEmpty())
          continue;
        CorrectionPreset preset;
        preset.name = name;
        preset.pitchCenter = juce::jlimit(0.0f, 100.0f, static_cast<float>(item.getProperty("pitchCenter", 100.0)));
        preset.pitchDrift = juce::jlimit(0.0f, 100.0f, static_cast<float>(item.getProperty("pitchDrift", 100.0)));
        preset.snapToScale = static_cast<bool>(item.getProperty("snapToScale", false));
        preset.keepVibrato = static_cast<bool>(item.getProperty("keepVibrato", true));
        const int engine = static_cast<int>(item.getProperty("engine", 0));
        preset.engine = engine >= 0 && engine <= static_cast<int>(NoteEngine::Classic)
                            ? static_cast<NoteEngine>(engine)
                            : NoteEngine::Default;
        presets.push_back(preset);
      }
    return presets;
  }

  static juce::var listToVar(const std::vector<CorrectionPreset> &presets)
  {
    juce::Array<juce::var> array;
    for (const auto &preset : presets)
      array.add(preset.toVar());
    return array;
  }
};

/**
 * The Pitch Center correction of one note (the popup's rule). `sourceMidi`
 * is the note's pitch before any macro, `meanShift` the step from it to the
 * mean of its voiced frames, `amount` 0..100 %. Snapping to the scale moves
 * the note that fraction of the way to its scale tone; otherwise the
 * deviation from the nearest semitone is capped, from 0.5 semitones at 0 %
 * down to none at 100 %.
 */
inline float pitchCenterCorrectedMidi(float sourceMidi, float meanShift, float amount,
                                      bool snapToScale, ScaleMode mode, int root,
                                      int referenceHz)
{
  const float maximumDeviation = (100.0f - amount) * 0.005f;
  const float sourceMean = sourceMidi + meanShift;
  const float corrected =
      (snapToScale ? ScaleUtils::snapMidiToScale(sourceMean, mode, root, referenceHz)
                   : std::round(sourceMean)) -
      meanShift;
  const float signedDeviation = sourceMidi - corrected;
  if (snapToScale)
    return corrected + signedDeviation * ((100.0f - amount) / 100.0f);
  if (std::abs(signedDeviation) > maximumDeviation)
    return corrected + std::copysign(maximumDeviation, signedDeviation);
  return sourceMidi;
}
