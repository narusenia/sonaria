#include "../Source/Utils/ScaleUtils.h"
#include "../Source/Undo/KeyChangeAction.h"
#include "../Source/Undo/PitchUndoManager.h"
#include "TestAssert.h"
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

  std::cout << "Key detection tests passed\n";
}
