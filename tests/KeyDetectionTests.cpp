#include "../Source/Utils/ScaleUtils.h"
#include "../Source/Undo/KeyChangeAction.h"
#include "../Source/Undo/PitchUndoManager.h"
#include "TestAssert.h"
#include <cmath>
#include <iostream>

static void addGMajorNotes(Project& project)
{
  int frame = 0;
  for (int midi : {67, 69, 71, 72, 74, 76, 78, 79, 67, 67}) {
    project.addNote(Note(frame, frame + 10, static_cast<float>(midi)));
    frame += 10;
  }
}

int main()
{
  // Analysis fills in the key when none is set, and the fill-in undoes in one step.
  Project project;
  addGMajorNotes(project);
  ScaleUtils::detectAndApplyScale(project);
  CHECK(project.getProjectKeyRoot() == 7 && project.getProjectKeyMode() == ScaleMode::Major);
  CHECK(project.getMacroParameters()->projectKeyInitialized);

  PitchUndoManager history;
  auto action = KeyChangeAction::takeAutoKey(project);
  CHECK(action != nullptr);
  CHECK(KeyChangeAction::takeAutoKey(project) == nullptr);
  history.addAction(std::move(action));
  history.undo();
  CHECK(project.getProjectKeyRoot() == 0 && !project.getMacroParameters()->projectKeyInitialized);
  history.redo();
  CHECK(project.getProjectKeyRoot() == 7 && project.getMacroParameters()->projectKeyInitialized);

  // A key set by hand survives re-analysis, which then records no undo step.
  project.setProjectKey(2, ScaleMode::Minor);
  ScaleUtils::detectAndApplyScale(project);
  CHECK(project.getProjectKeyRoot() == 2 && project.getProjectKeyMode() == ScaleMode::Minor);
  CHECK(project.getDetectedScaleRootNote() == 7);
  CHECK(KeyChangeAction::takeAutoKey(project) == nullptr);

  // Arrow keys step along the scale: E4 up in C major is F4, B4 up is C5, C4 down is B3.
  CHECK(ScaleUtils::stepMidiOnScale(64.0f, 1, ScaleMode::Major, 0) == 65.0f);
  CHECK(ScaleUtils::stepMidiOnScale(71.0f, 1, ScaleMode::Major, 0) == 72.0f);
  CHECK(ScaleUtils::stepMidiOnScale(60.0f, -1, ScaleMode::Major, 0) == 59.0f);
  // Off-scale or between tones, the step lands on the next tone in that direction.
  CHECK(ScaleUtils::stepMidiOnScale(66.3f, 1, ScaleMode::Major, 0) == 67.0f);
  CHECK(ScaleUtils::stepMidiOnScale(66.3f, -1, ScaleMode::Major, 0) == 65.0f);
  CHECK(ScaleUtils::stepMidiOnScale(64.005f, 1, ScaleMode::Major, 0) == 65.0f);
  CHECK(ScaleUtils::stepMidiOnScale(61.0f, 1, ScaleMode::Chromatic, 0) == 62.0f);

  // Degree shifts in C major: E up a third is G, F up a third is A, G down a third is E.
  CHECK(ScaleUtils::shiftByDegree(64.0f, 3, ScaleMode::Major, 0).midi == 67.0f);
  CHECK(ScaleUtils::shiftByDegree(65.0f, 3, ScaleMode::Major, 0).midi == 69.0f);
  CHECK(ScaleUtils::shiftByDegree(67.0f, -3, ScaleMode::Major, 0).midi == 64.0f);
  CHECK(ScaleUtils::shiftByDegree(71.0f, 2, ScaleMode::Major, 0).midi == 72.0f);
  CHECK(ScaleUtils::shiftByDegree(60.0f, 8, ScaleMode::Major, 0).midi == 72.0f);
  CHECK(ScaleUtils::shiftByDegree(60.0f, -8, ScaleMode::Major, 0).midi == 48.0f);
  // Cents ride along: E4 +20 ct up a third is G4 +20 ct.
  CHECK(std::abs(ScaleUtils::shiftByDegree(64.2f, 3, ScaleMode::Major, 0).midi - 67.2f) < 1e-4f);
  // Off-scale F#4 moves from F4: F up a third is A, so F# lands on A# and is flagged.
  const auto offScale = ScaleUtils::shiftByDegree(66.0f, 3, ScaleMode::Major, 0);
  CHECK(offScale.midi == 70.0f && offScale.outOfScale);
  CHECK(!ScaleUtils::shiftByDegree(64.0f, 3, ScaleMode::Major, 0).outOfScale);
  // A minor: C up a third is E, A up a third is C.
  CHECK(ScaleUtils::shiftByDegree(60.0f, 3, ScaleMode::Minor, 9).midi == 64.0f);
  CHECK(ScaleUtils::shiftByDegree(57.0f, 3, ScaleMode::Minor, 9).midi == 60.0f);

  std::cout << "Key detection tests passed\n";
}
