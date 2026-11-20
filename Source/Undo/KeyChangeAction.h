#pragma once

#include "UndoableAction.h"
#include "../Models/Project.h"

/**
 * Restores the project key that analysis filled in on its own.
 */
class KeyChangeAction final : public UndoableAction
{
public:
    KeyChangeAction(std::shared_ptr<MacroParameters> macros, ProjectKeyState oldKey, ProjectKeyState newKey)
        : target(std::move(macros)), before(oldKey), after(newKey) {}

    // The undo step for a key analysis just seeded, or nullptr if it seeded none.
    static std::unique_ptr<UndoableAction> takeAutoKey(Project& project)
    {
        const auto before = project.takeAutoKeyBefore();
        auto macros = project.getMacroParameters();
        if (!before || !macros)
            return nullptr;
        const ProjectKeyState after{macros->projectKeyRoot, macros->projectKeyMode, macros->projectKeyInitialized};
        return std::make_unique<KeyChangeAction>(std::move(macros), *before, after);
    }

    void undo() override { apply(before); }
    void redo() override { apply(after); }
    juce::String getName() const override { return "Detect Key"; }
    bool requiresAudioResynthesis() const override { return false; }

private:
    void apply(const ProjectKeyState& state)
    {
        target->projectKeyRoot = state.root;
        target->projectKeyMode = state.mode;
        target->projectKeyInitialized = state.initialized;
    }

    std::shared_ptr<MacroParameters> target;
    ProjectKeyState before, after;
};
