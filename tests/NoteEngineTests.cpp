#include "../Source/Models/ProjectSerializer.h"
#include "../Source/Utils/TransformParams.h"
#include "TestAssert.h"
#include <iostream>

int main()
{
  Project project;
  project.addNote(Note(0, 10, 60.0f));
  project.addNote(Note(10, 20, 62.0f));
  project.addNote(Note(20, 30, 64.0f));
  project.getNotes()[0].setEngine(NoteEngine::AI);
  project.getNotes()[1].setEngine(NoteEngine::Classic);

  // JSON (.pnproj) and the binary archive (plug-in state / ARA) both keep the choice.
  Project fromJson;
  CHECK(ProjectSerializer::fromJson(fromJson, ProjectSerializer::toJson(project)));
  CHECK(fromJson.getNotes()[0].getEngine() == NoteEngine::AI);
  CHECK(fromJson.getNotes()[1].getEngine() == NoteEngine::Classic);
  CHECK(fromJson.getNotes()[2].getEngine() == NoteEngine::Default);

  juce::MemoryBlock archive;
  CHECK(ProjectSerializer::toBinaryArchive(
      project, archive, ProjectSerializer::BinaryArchiveMode::selfContained));
  Project fromArchive;
  CHECK(ProjectSerializer::fromBinaryArchive(fromArchive, archive.getData(), archive.getSize()));
  CHECK(fromArchive.getNotes()[1].getEngine() == NoteEngine::Classic);

  // Projects saved before per-note engines, or naming an unknown engine, read as Default.
  auto legacy = ProjectSerializer::toJson(project);
  auto *notes = legacy.getDynamicObject()->getProperty("notes").getArray();
  notes->getReference(0).getDynamicObject()->removeProperty("engine");
  notes->getReference(1).getDynamicObject()->setProperty("engine", "Spectral");
  Project fromLegacy;
  CHECK(ProjectSerializer::fromJson(fromLegacy, legacy));
  CHECK(fromLegacy.getNotes()[0].getEngine() == NoteEngine::Default);
  CHECK(fromLegacy.getNotes()[1].getEngine() == NoteEngine::Default);

  // Undo snapshots restore the engine.
  Note note(0, 10, 60.0f);
  const auto before = TransformParams::fromNote(note);
  note.setEngine(NoteEngine::Classic);
  CHECK(TransformParams::fromNote(note) != before);
  before.applyToNote(note);
  CHECK(note.getEngine() == NoteEngine::Default);

  std::cout << "Note engine tests passed\n";
}
