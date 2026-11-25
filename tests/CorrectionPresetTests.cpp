#include "../Source/Utils/CorrectionPreset.h"
#include "TestAssert.h"
#include <iostream>

int main()
{
  // Presets survive the settings file, and bad entries are dropped or clamped.
  CorrectionPreset preset;
  preset.name = "Lead";
  preset.pitchCenter = 80.0f;
  preset.pitchDrift = 40.0f;
  preset.snapToScale = true;
  preset.keepVibrato = false;
  preset.engine = NoteEngine::Classic;
  const auto stored = juce::JSON::toString(CorrectionPreset::listToVar({preset}));
  const auto loaded = CorrectionPreset::listFromVar(juce::JSON::parse(stored));
  CHECK(loaded.size() == 1);
  CHECK(loaded[0].name == "Lead" && loaded[0].pitchCenter == 80.0f && loaded[0].pitchDrift == 40.0f);
  CHECK(loaded[0].snapToScale && !loaded[0].keepVibrato && loaded[0].engine == NoteEngine::Classic);

  const auto odd = CorrectionPreset::listFromVar(juce::JSON::parse(
      R"([{"name": "  "}, {"name": "X", "pitchCenter": 250, "engine": 9}])"));
  CHECK(odd.size() == 1 && odd[0].name == "X" && odd[0].pitchCenter == 100.0f);
  CHECK(odd[0].engine == NoteEngine::Default && odd[0].keepVibrato);
  CHECK(CorrectionPreset::listFromVar({}).empty());

  // The Pitch Center rule (chromatic): 100% lands the mean on the semitone,
  // 0% keeps a note within half a semitone, 50% caps the deviation at 25 ct.
  const auto chromatic = [](float midi, float amount)
  { return pitchCenterCorrectedMidi(midi, 0.0f, amount, false, ScaleMode::Chromatic, 0, 440); };
  CHECK(std::abs(chromatic(60.4f, 100.0f) - 60.0f) < 1e-4f);
  CHECK(std::abs(chromatic(60.4f, 0.0f) - 60.4f) < 1e-4f);
  CHECK(std::abs(chromatic(60.4f, 50.0f) - 60.25f) < 1e-4f);
  CHECK(std::abs(chromatic(60.1f, 50.0f) - 60.1f) < 1e-4f);
  // The mean, not the note, is what lands on the semitone.
  CHECK(std::abs(pitchCenterCorrectedMidi(60.0f, 0.3f, 100.0f, false, ScaleMode::Chromatic, 0, 440) - 59.7f) < 1e-4f);
  // Snapping goes that fraction of the way to the scale tone.
  CHECK(std::abs(pitchCenterCorrectedMidi(60.4f, 0.0f, 50.0f, true, ScaleMode::Chromatic, 0, 440) - 60.2f) < 1e-4f);

  std::cout << "Correction preset tests passed\n";
}
