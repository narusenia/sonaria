#pragma once

#include "../../JuceHeader.h"
#include "../../Models/Project.h"
#include "../../Undo/UndoActions.h"
#include "CoordinateMapper.h"
#include <functional>
#include <memory>
#include <utility>
#include <vector>

/**
 * Handles note splitting operations.
 */
class NoteSplitter {
public:
    NoteSplitter() = default;
    ~NoteSplitter() = default;

    void setProject(Project* proj) { project = proj; }
    void setUndoManager(PitchUndoManager* manager) { undoManager = manager; }
    void setCoordinateMapper(CoordinateMapper* mapper) { coordMapper = mapper; }

    /**
     * Find note at the given world coordinates.
     */
    Note* findNoteAt(float x, float y);

    /**
     * Split a note at the given frame position.
     * Returns true if split was successful.
     */
    bool splitNoteAtFrame(Note* note, int splitFrame);

    /**
     * Splits several notes, each (note start frame, split frames), as one
     * undo step. Returns true if any split was made.
     */
    bool splitNotesAtFrames(const std::vector<std::pair<int, std::vector<int>>>& splits);

    /**
     * Split note at world X coordinate.
     * Returns true if split was successful.
     */
    bool splitNoteAtX(Note* note, float x);

    /** Returns the adjacent note pair represented by a split boundary at x/y. */
    std::pair<Note*, Note*> findMergeableNotesAt(float x, float y);

    /** Merges two adjacent segments that originated from the same note. */
    bool mergeNotes(Note* first, Note* second);

    // Callbacks
    std::function<void()> onNoteSplit;

private:
    // The split itself; its undo action, or nullptr when the frame is outside.
    std::unique_ptr<UndoableAction> performSplit(Note* note, int splitFrame);

    Project* project = nullptr;
    PitchUndoManager* undoManager = nullptr;
    CoordinateMapper* coordMapper = nullptr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NoteSplitter)
};
