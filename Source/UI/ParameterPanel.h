#pragma once

#include "../JuceHeader.h"
#include "../Audio/SynthesisEngineType.h"
#include "../Audio/IO/MidiReference.h"
#include "../Models/Note.h"
#include "../Models/Project.h"
#include "../Undo/UndoActions.h"
#include "../Utils/Localization.h"
#include "../Utils/UI/Theme.h"
#include "IMainView.h"
#include "StyledComponents.h"
#include "Sliders.h"
#include "Components/MidiDragButton.h"
#include "Workspace/PanelContent.h"
#include <optional>
#include <vector>

class ParameterPanel : public juce::Component,
                       public juce::Button::Listener,
                       public PanelContent,
                       private juce::Timer
{
public:
    ParameterPanel();
    ~ParameterPanel() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void buttonClicked(juce::Button* button) override;

    void setProject(Project* proj);
    void setPluginMode(bool pluginMode);

    /** Re-applies every caption and tooltip after a language change. */
    void refreshLocalisedText();

    void setHostTimelineState(double bpm, int numerator, int denominator);
    void setUndoManager(PitchUndoManager* mgr) { juce::ignoreUnused(mgr); }
    void setSelectedNote(Note* note);
    void setSynthesisEngine(SynthesisEngineType type);
    SynthesisEngineType getSynthesisEngine() const { return synthesisEngine; }
    // Disables the AI Resynthesis option on CPU-only machines.
    void setAiResynthesisAvailable(bool available);

    // ===== Rendering card: inference device =====
    // The row only earns its space when there is a choice to make, so it shows
    // only while AI Resynthesis is the engine and the execution provider
    // exposes more than one device. Pass a list of one - or none - to hide it.
    void setRenderDeviceOptions(const juce::StringArray& deviceNames,
                                int selectedIndex);
    void updateFromNote();
    void updateGlobalSliders();
    std::function<void(Project*)> onProjectBound;

    // Height the cards need at full size. The hosting panel scrolls the
    // content when it has less room than this instead of squeezing the cards.
    int getPreferredHeight() const override;

    std::function<void()> onParameterChanged;

    // The button at the bottom of the panel; app settings live in that dialog.
    std::function<void()> onShowSettingsClicked;
    std::function<void()> onPreferredHeightChanged;
    std::function<void()> onParameterEditFinished;
    std::function<void(int)> onScaleRootChanged;
    std::function<void(ScaleMode)> onScaleModeChanged;
    std::function<void(bool)> onSnapToSemitonesChanged;
    std::function<void(DragSnapMode)> onDragSnapModeChanged;
    std::function<void()> onDoubleClickSnapModeChanged;
    std::function<void(int)> onPitchReferenceChanged;
    std::function<void(TimelineDisplayMode)> onTimelineDisplayModeChanged;
    std::function<void(int, int)> onTimelineBeatSignatureChanged;
    std::function<void(double)> onTimelineTempoChanged;
    std::function<void(TimelineGridDivision)> onTimelineGridDivisionChanged;
    std::function<void(bool)> onTimelineSnapCycleChanged;
    std::function<void(SynthesisEngineType)> onSynthesisEngineChanged;
    std::function<void(int)> onRenderDeviceChanged;

    // Selected note card. Each call is one edit of the whole selection, so
    // the owner can record it as one undo step.
    std::function<void(NoteEngine)> onSelectedNotesEngineChanged;
    std::function<void(float)> onSelectedNotesVolumeChanged;
    std::function<void(float)> onSelectedNotesFormantChanged;
    // Align to the pinned tracks at the card's strength (0..1).
    std::function<void(float)> onAlignToReference;
    float getAlignStrength() const { return static_cast<float>(noteAlignSlider.getValue() / 100.0); }

    // Correction presets: the owner keeps them; the card shows their names.
    void setCorrectionPresetNames(const juce::StringArray& names, const juce::String& selected);
    std::function<void(const juce::String&)> onApplyCorrectionPreset;
    std::function<void(const juce::String&)> onSaveCorrectionPreset; // new or same name
    std::function<void(const juce::String&)> onDeleteCorrectionPreset;

    // MIDI card. Dragging the button asks the owner for a freshly written
    // .mid file and hands it to the OS as a file drag; a click saves instead.
    std::function<juce::File()> onCreateMidiDragFile;
    std::function<void()> onExportMidiClicked;
    std::function<void()> onOpenMidiReference;
    std::function<void(const std::vector<MidiReference::Note>&, bool)> onMidiReferenceChanged;
    std::function<void()> onMidiReferenceEdited;
    void setMidiReference(juce::String filename, std::vector<MidiReference::Track> tracks);
    void refreshMidiReference();

private:
    void timerCallback() override;
    void refreshNoteCard();
    void setupTextButton(juce::TextButton& button);
    void showDragSnapModeMenu();
    void showDoubleClickSnapModeMenu();
    void showTimelineBeatMenu();
    void showTimelineGridMenu();
    void showMidiReferenceTrackMenu();
    void refreshMidiReferenceControls();
    void editMidiReference(MidiReference::State state);

    void setScaleRootInternal(int rootNote, bool notify);
    void setScaleModeInternal(ScaleMode mode, bool notify);
    void setSnapToSemitonesInternal(bool enabled, bool notify);
    void setDragSnapModeInternal(DragSnapMode mode, bool notify);
    void setPitchReferenceInternal(int hz, bool notify);
    void refreshModeToggles();
    void refreshSynthesisToggles();
    void refreshRenderDeviceRow();
    void showRenderDeviceMenu();
    void setSynthesisEngineInternal(SynthesisEngineType type, bool notify);
    void refreshTimelineModeToggles();
    void setTimelineDisplayModeInternal(TimelineDisplayMode mode, bool notify);
    void setTimelineBeatSignatureInternal(int numerator, int denominator, bool notify);
    void setTimelineTempoBpmInternal(double bpm, bool notify);
    void setTimelineGridDivisionInternal(TimelineGridDivision division, bool notify);
    void setTimelineSnapCycleInternal(bool enabled, bool notify);

    Project* project = nullptr;
    Note* selectedNote = nullptr;
    bool isUpdating = false;

    // Card titles and control captions all come from the string table; see
    // refreshLocalisedText(), which is also what a language change calls.
    // Selected note card: shown only while non-rest notes are selected.
    juce::Label noteSectionLabel;
    juce::Rectangle<int> noteCardBounds;
    bool noteCardVisible = false;
    juce::Label noteEngineLabel;
    SegmentedControl noteEngineControl; // Default | AI | Classic, as NoteEngine
    juce::Label noteDeviationLabel;
    juce::Label noteDeviationValue;
    juce::Label noteVolumeLabel;
    SliderBox noteVolumeSlider { "Volume" };
    juce::Label noteFormantLabel;
    SliderBox noteFormantSlider { "Formant" };
    juce::Label noteAlignLabel;
    SliderBox noteAlignSlider { "Strength" };
    juce::TextButton noteAlignButton;
    juce::Label notePresetLabel;
    ComboSelectionButton notePresetButton;
    juce::TextButton notePresetApplyButton, notePresetSaveButton, notePresetDeleteButton;
    juce::StringArray presetNames;
    juce::String selectedPresetName;
    void showPresetMenu();
    void askPresetName();
    void refreshPresetControls();
    // A slider drag edits the notes once, on release, so it is one undo step.
    bool noteSliderDragging = false;

    juce::Label pitchSectionLabel;
    juce::Rectangle<int> pitchCardBounds;
    juce::Label timeSectionLabel;
    juce::Rectangle<int> timeCardBounds;
    juce::Label synthesisSectionLabel;
    juce::Rectangle<int> synthesisCardBounds;
    MidiDragButton midiDragButton;
    juce::Rectangle<int> midiReferenceCardBounds;
    juce::Label midiReferenceSectionLabel;
    MidiFileButton midiReferenceFileButton;
    ComboSelectionButton midiReferenceTrackButton;
    StyledToggleButton midiReferenceVisibilityToggle;
    unsigned midiReferenceRevision = 0;

    CompactSelectionButton settingsButton;

    SegmentedControl pitchModeControl; // Chromatic | Scale

    // How an edited region is rendered. Only one of these is resynthesis in
    // the analysis/resynthesis sense - the model turns a spectrum back into a
    // signal, while PSOLA never leaves the time domain and emits the original
    // samples, windowed and repositioned. Hence "Rendering" as the umbrella
    // and the asymmetric labels.
    //
    // Not ranked by quality: neither is better everywhere. The model wins on
    // large pitch moves, PSOLA returns untouched audio bit for bit.
    SegmentedControl renderingControl; // AI Resynthesis (Vocoder) | Classic (PSOLA)

    juce::Label renderDeviceLabel;
    ComboSelectionButton renderDeviceButton;

    juce::Label referenceLabel;
    SliderBox referenceSlider { "Pitch Reference" };

    StyledToggleButton snapToSemitonesToggle;
    CompactSelectionButton dragSnapModeButton;
    juce::Label doubleClickSnapLabel;
    CompactSelectionButton doubleClickSnapBox;

    SegmentedControl timelineModeControl; // Beats | Time
    juce::Label timelineBeatLabel;
    CompactSelectionButton timelineBeatButton { "4/4" };
    juce::Label timelineTempoLabel;
    SliderBox timelineTempoSlider { "Tempo" };
    juce::Label timelineGridLabel;
    CompactSelectionButton timelineGridButton { "1/4" };
    StyledToggleButton timelineSnapCycleToggle;

    int selectedScaleRootNote = 0;
    ScaleMode selectedScaleMode = ScaleMode::Chromatic;
    ScaleMode lastNonChromaticMode = ScaleMode::Major;
    bool snapToSemitones = false;
    DragSnapMode dragSnapMode = DragSnapMode::Chromatic;
    int pitchReferenceHz = 440;
    TimelineDisplayMode timelineDisplayMode = TimelineDisplayMode::Beats;
    int timelineBeatNumerator = 4;
    int timelineBeatDenominator = 4;
    double timelineTempoBpm = 120.0;
    TimelineGridDivision timelineGridDivision = TimelineGridDivision::Quarter;
    bool timelineSnapCycle = false;
    SynthesisEngineType synthesisEngine = defaultSynthesisEngineType();
    bool aiResynthesisAvailable = true;

    juce::StringArray renderDeviceNames;
    int renderDeviceIndex = 0;
    bool renderDeviceRowVisible = false;

    // Declared last so it is torn down before the widgets it refreshes.
    LocalisationWatcher languageWatcher{[this]
                                        {
                                            refreshLocalisedText();
                                            resized();
                                            repaint();
                                        }};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ParameterPanel)
};
