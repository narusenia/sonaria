#include "ParameterPanel.h"
#include "../Audio/IO/MidiExporter.h"
#include "../Utils/Localization.h"
#include "../Utils/NoteDeviation.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <limits>

namespace
{
constexpr bool kShowPitchCard = true;

// =========================================================================
// LAYOUT METRICS
//
// resized() lays the cards out strictly top down at these fixed sizes, so the
// height the panel needs is a sum of them rather than a function of the space
// it is given. kPreferredPanelHeight is that sum; the hosting panel uses it to
// decide when to scroll. resized() asserts the two agree, so changing a row
// height here without updating the sum is caught in a debug build.
// =========================================================================
constexpr int kCardPadX = 5;   // horizontal padding outside cards
constexpr int kCardPadY = 6;   // vertical padding outside cards
constexpr int kCardGap = 6;    // gap between cards
constexpr int kInnerPadX = 10; // padding inside each card
constexpr int kInnerPadY = 8;  // padding inside each card
constexpr int kRowGap = 5;
constexpr int kColumnGap = 6;

constexpr int kSectionLabelHeight = 20;
constexpr int kSectionLabelGap = kRowGap + 2;
constexpr int kSegmentedRowHeight = 26;     // Rendering / Time / Pitch mode
constexpr int kControlRowHeight = 26;       // label + control rows
constexpr int kMidiButtonHeight = 28;
constexpr int kMidiExportRowGap = kRowGap + 1;

constexpr int kSynthesisCardHeight =
    kInnerPadY + kSectionLabelHeight + kSectionLabelGap + kSegmentedRowHeight + kInnerPadY;

// The inference device row is optional: it adds itself, leading gap included,
// only while it is showing.
constexpr int kRenderDeviceRowExtraHeight = (kRowGap + 1) + kControlRowHeight;
constexpr int kTimeCardHeight =
    kInnerPadY + kSectionLabelHeight + kSectionLabelGap + kSegmentedRowHeight +
    (kRowGap + 1) + kControlRowHeight + (kRowGap + kControlRowHeight) * 2 + kInnerPadY;
constexpr int kPitchCardHeight =
    kInnerPadY + kSectionLabelHeight + kSectionLabelGap + kSegmentedRowHeight +
    ((kRowGap + 1) + kControlRowHeight) * 3 + kInnerPadY;
// Only counted while notes are selected; see getPreferredHeight().
constexpr int kNoteCardHeight =
    kInnerPadY + kSectionLabelHeight + kSectionLabelGap + kSegmentedRowHeight +
    ((kRowGap + 1) + kControlRowHeight) * 7 + kInnerPadY;
constexpr int kMidiReferenceCardHeight =
    kInnerPadY * 2 + kSectionLabelHeight + kSectionLabelGap +
    kMidiButtonHeight * 2 + kControlRowHeight + kMidiExportRowGap + kRowGap;

// Four cards with a gap between each, then the settings button.
constexpr int kPreferredPanelHeight =
    kCardPadY * 2 + kCardGap * 4 + kSynthesisCardHeight + kTimeCardHeight +
    kPitchCardHeight + kMidiReferenceCardHeight + kMidiButtonHeight;

constexpr int kRenderDeviceMenuBaseId = 7501;



struct TimelineBeatOption
{
    int numerator = 4;
    int denominator = 4;
    const char* label = "4/4";
};

struct TimelineGridOption
{
    TimelineGridDivision division = TimelineGridDivision::Quarter;
    const char* label = "1/4";
};

constexpr std::array<TimelineBeatOption, 6> kTimelineBeatOptions {{
    { 3, 4, "3/4" },
    { 4, 4, "4/4" },
    { 5, 4, "5/4" },
    { 6, 8, "6/8" },
    { 7, 8, "7/8" },
    { 12, 8, "12/8" }
}};

constexpr std::array<TimelineGridOption, 6> kTimelineGridOptions {{
    { TimelineGridDivision::Whole, "1/1" },
    { TimelineGridDivision::Half, "1/2" },
    { TimelineGridDivision::Quarter, "1/4" },
    { TimelineGridDivision::Eighth, "1/8" },
    { TimelineGridDivision::Sixteenth, "1/16" },
    { TimelineGridDivision::ThirtySecond, "1/32" }
}};

int normalizeTimelineBeatDenominator(int denominator)
{
    denominator = juce::jlimit(1, 32, denominator);
    int normalized = 1;
    while (normalized < denominator)
        normalized <<= 1;
    const int lower = normalized >> 1;
    if (lower >= 1 && (denominator - lower) < (normalized - denominator))
        normalized = lower;
    return juce::jlimit(1, 32, normalized);
}

juce::String getTimelineBeatLabel(int numerator, int denominator)
{
    const int normalizedNumerator = juce::jlimit(1, 32, numerator);
    const int normalizedDenominator = normalizeTimelineBeatDenominator(denominator);
    return juce::String(normalizedNumerator) + "/" + juce::String(normalizedDenominator);
}

juce::String getTimelineGridLabel(TimelineGridDivision division)
{
    for (const auto& option : kTimelineGridOptions)
        if (option.division == division)
            return option.label;
    return "1/4";
}

juce::String getDragSnapModeLabel(DragSnapMode mode)
{
    return mode == DragSnapMode::Scale ? TR("param.scale") : TR("param.chromatic");
}

juce::String getDoubleClickSnapModeLabel(DoubleClickSnapMode mode)
{
    switch (mode)
    {
        case DoubleClickSnapMode::PitchCenter: return TR("quantize.pitch_center");
        case DoubleClickSnapMode::Scale: return TR("param.scale");
        case DoubleClickSnapMode::MidiReference: return TR("param.midi_ref");
    }
    return {};
}

juce::String getMidiReferenceTrackLabel(const MidiReference::Track& track)
{
    auto label = TR("param.midi_reference_track") + " " + juce::String(track.number);
    if (track.name.isNotEmpty())
        label += "  " + track.name;
    return label;
}

// Device names arrive provider-tagged ("Radeon RX 7900 XT (DirectML)") because
// the Settings list mixes providers. In this card they never do - the provider
// is fixed - so the tag is dead weight in a 150px button. Strip it here rather
// than at the source, so Settings keeps the disambiguation it needs.
juce::String getRenderDeviceLabel(const juce::String& deviceName)
{
    const int suffixStart = deviceName.lastIndexOfChar('(');
    if (suffixStart > 0 && deviceName.trimEnd().endsWithChar(')'))
        return deviceName.substring(0, suffixStart).trimEnd();
    return deviceName;
}

class PitchPopupLookAndFeel final : public juce::LookAndFeel_V4
{
public:
    PitchPopupLookAndFeel()
    {
        setColour(juce::PopupMenu::backgroundColourId, juce::Colours::transparentBlack);
        setColour(juce::PopupMenu::textColourId, APP_COLOR_TEXT_PRIMARY);
        setColour(juce::PopupMenu::highlightedBackgroundColourId,
                  APP_COLOR_MENU_ITEM_HIGHLIGHT);
        setColour(juce::PopupMenu::highlightedTextColourId, APP_COLOR_TEXT_PRIMARY);
    }

    void drawPopupMenuBackground(juce::Graphics& g, int width, int height) override
    {
        const auto bounds = juce::Rectangle<float>(
            0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height));
        const auto area = bounds.reduced(0.5f);
        juce::Path shape;
        shape.addRoundedRectangle(area, 9.0f);

        g.setColour(APP_COLOR_MENU_BACKGROUND);
        g.fillPath(shape);
        g.setColour(APP_COLOR_MENU_BORDER);
        g.strokePath(shape, juce::PathStrokeType(1.0f));
    }

    void drawResizableFrame(juce::Graphics&, int, int,
                            const juce::BorderSize<int>&) override
    {
        // PopupMenu draws this frame when a parent component is provided.
        // The default implementation is a square outline that breaks rounded corners.
    }
};

PitchPopupLookAndFeel& getPitchPopupLookAndFeel()
{
    static PitchPopupLookAndFeel lookAndFeel;
    return lookAndFeel;
}

class HoverMenuItemComponent final : public juce::PopupMenu::CustomComponent
{
public:
    HoverMenuItemComponent(juce::String text, bool selected, std::function<void()> hoverCallback = {})
        : juce::PopupMenu::CustomComponent(true),
          itemText(std::move(text)),
          isSelected(selected),
          onHover(std::move(hoverCallback))
    {
        setOpaque(false);
    }

    void getIdealSize(int& idealWidth, int& idealHeight) override
    {
        const int textWidth = juce::GlyphArrangement::getStringWidthInt(
            AppFont::getFont(14.0f), itemText);
        idealWidth = textWidth + 44; // matching 22 px left/right insets
        idealHeight = 26;
    }

    void paint(juce::Graphics& g) override
    {
        const auto area = getLocalBounds().toFloat();
        if (isItemHighlighted())
        {
            g.setColour(APP_COLOR_MENU_ITEM_HIGHLIGHT);
            g.fillRoundedRectangle(area.reduced(2.0f, 1.0f), 5.0f);
        }

        if (isSelected)
        {
            g.setColour(APP_COLOR_MENU_TICK);
            g.fillEllipse(8.0f, area.getCentreY() - 3.5f, 7.0f, 7.0f);
        }

        g.setColour(APP_COLOR_TEXT_PRIMARY);
        g.setFont(AppFont::getFont(14.0f));
        g.drawText(itemText, getLocalBounds().withTrimmedLeft(22),
                   juce::Justification::centredLeft, true);
    }

    void mouseEnter(const juce::MouseEvent& e) override
    {
        juce::PopupMenu::CustomComponent::mouseEnter(e);
        if (onHover)
            onHover();
    }

    void mouseMove(const juce::MouseEvent& e) override
    {
        juce::PopupMenu::CustomComponent::mouseMove(e);
        if (onHover)
            onHover();
    }

private:
    juce::String itemText;
    bool isSelected = false;
    std::function<void()> onHover;
};
}

ParameterPanel::ParameterPanel()
{
    // Selected note card, added hidden until a note is selected.
    for (auto* label : { &noteSectionLabel, &noteEngineLabel, &noteDeviationLabel,
                         &noteDeviationValue, &noteVolumeLabel, &noteFormantLabel,
                         &noteAlignLabel, &notePresetLabel })
        addChildComponent(label);
    noteSectionLabel.setColour(juce::Label::textColourId, APP_COLOR_PANEL_SECTION_TITLE);
    noteSectionLabel.setFont(AppFont::getBoldFont(16.0f));
    for (auto* label : { &noteEngineLabel, &noteDeviationLabel, &noteVolumeLabel, &noteFormantLabel,
                         &noteAlignLabel, &notePresetLabel })
    {
        label->setColour(juce::Label::textColourId, APP_COLOR_PANEL_LABEL);
        label->setFont(AppFont::getFont(14.0f));
    }
    noteDeviationValue.setColour(juce::Label::textColourId, APP_COLOR_CONTROL_TEXT);
    noteDeviationValue.setFont(AppFont::getFont(14.0f));

    addChildComponent(noteEngineControl);
    noteEngineControl.onChange = [this](int index)
    {
        if (onSelectedNotesEngineChanged)
            onSelectedNotesEngineChanged(static_cast<NoteEngine>(index));
    };

    const auto setupNoteSlider = [this](SliderBox& slider, double minimum, double maximum,
                                        const juce::String& suffix,
                                        std::function<void(float)> ParameterPanel::* callback)
    {
        slider.setRange(minimum, maximum, 0.1);
        slider.setValue(0.0, juce::dontSendNotification);
        slider.setDoubleClickReturnValue(true, 0.0);
        slider.setNumDecimalPlacesToDisplay(1);
        slider.setTextValueSuffix(suffix);
        slider.setHideSuffixWhileEditing(true);
        slider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, 64, 22);
        // The wheel scrolls the panel instead; each wheel step would be its own undo.
        slider.setScrollWheelEnabled(false);
        slider.setColour(juce::Slider::backgroundColourId, APP_COLOR_CONTROL_FILL);
        slider.setColour(juce::Slider::trackColourId, APP_COLOR_CONTROL_FILL);
        slider.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colours::transparentBlack);
        slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
        slider.setColour(juce::Slider::textBoxTextColourId, APP_COLOR_CONTROL_TEXT);
        const auto commit = [this, &slider, callback]
        {
            if (this->*callback)
                (this->*callback)(static_cast<float>(slider.getValue()));
        };
        slider.onDragStart = [this] { noteSliderDragging = true; };
        slider.onDragEnd = [this, commit]
        {
            noteSliderDragging = false;
            commit();
        };
        // Typed values and double-click resets arrive here outside a drag.
        slider.onValueChange = [this, commit]
        {
            if (!isUpdating && !noteSliderDragging)
                commit();
        };
        addChildComponent(slider);
    };
    setupNoteSlider(noteVolumeSlider, -60.0, 24.0, " dB",
                    &ParameterPanel::onSelectedNotesVolumeChanged);
    setupNoteSlider(noteFormantSlider, -12.0, 12.0, " st",
                    &ParameterPanel::onSelectedNotesFormantChanged);

    // Timing align: the slider is only the strength; the button applies it.
    noteAlignSlider.setRange(0.0, 100.0, 1.0);
    noteAlignSlider.setValue(100.0, juce::dontSendNotification);
    noteAlignSlider.setDoubleClickReturnValue(true, 100.0);
    noteAlignSlider.setNumDecimalPlacesToDisplay(0);
    noteAlignSlider.setTextValueSuffix(" %");
    noteAlignSlider.setHideSuffixWhileEditing(true);
    noteAlignSlider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, 64, 22);
    noteAlignSlider.setScrollWheelEnabled(false);
    for (auto id : { juce::Slider::backgroundColourId, juce::Slider::trackColourId })
        noteAlignSlider.setColour(id, APP_COLOR_CONTROL_FILL);
    noteAlignSlider.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colours::transparentBlack);
    noteAlignSlider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    noteAlignSlider.setColour(juce::Slider::textBoxTextColourId, APP_COLOR_CONTROL_TEXT);
    addChildComponent(noteAlignSlider);
    setupTextButton(noteAlignButton);
    noteAlignButton.onClick = [this]
    {
        if (onAlignToReference)
            onAlignToReference(getAlignStrength());
    };
    addChildComponent(noteAlignButton);

    addChildComponent(notePresetButton);
    notePresetButton.onClick = [this] { showPresetMenu(); };
    for (auto* button : { &notePresetApplyButton, &notePresetSaveButton, &notePresetDeleteButton })
    {
        setupTextButton(*button);
        addChildComponent(button);
    }
    notePresetApplyButton.onClick = [this]
    {
        if (selectedPresetName.isNotEmpty() && onApplyCorrectionPreset)
            onApplyCorrectionPreset(selectedPresetName);
    };
    notePresetSaveButton.onClick = [this] { askPresetName(); };
    notePresetDeleteButton.onClick = [this]
    {
        if (selectedPresetName.isNotEmpty() && onDeleteCorrectionPreset)
            onDeleteCorrectionPreset(selectedPresetName);
    };
    refreshPresetControls();

    addAndMakeVisible(pitchSectionLabel);
    pitchSectionLabel.setColour(juce::Label::textColourId, APP_COLOR_PANEL_SECTION_TITLE);
    pitchSectionLabel.setFont(AppFont::getBoldFont(16.0f));
    addAndMakeVisible(doubleClickSnapLabel);
    doubleClickSnapLabel.setFont(AppFont::getFont(14.0f));
    doubleClickSnapLabel.setBorderSize({1, 5, 1, 0});
    doubleClickSnapLabel.setColour(juce::Label::textColourId, APP_COLOR_TEXT_PRIMARY);
    addAndMakeVisible(doubleClickSnapBox);
    doubleClickSnapBox.setEnabled(false);
    doubleClickSnapBox.onClick = [this] { showDoubleClickSnapModeMenu(); };
    addAndMakeVisible(timeSectionLabel);
    timeSectionLabel.setColour(juce::Label::textColourId, APP_COLOR_PANEL_SECTION_TITLE);
    timeSectionLabel.setFont(AppFont::getBoldFont(16.0f));
    addAndMakeVisible(synthesisSectionLabel);
    synthesisSectionLabel.setColour(juce::Label::textColourId, APP_COLOR_PANEL_SECTION_TITLE);
    synthesisSectionLabel.setFont(AppFont::getBoldFont(16.0f));
    addAndMakeVisible(midiReferenceSectionLabel);
    midiReferenceSectionLabel.setColour(juce::Label::textColourId, APP_COLOR_PANEL_SECTION_TITLE);
    midiReferenceSectionLabel.setFont(AppFont::getBoldFont(16.0f));
    addAndMakeVisible(midiReferenceFileButton);
    midiReferenceFileButton.onClick = [this]
    {
        if (onOpenMidiReference)
            onOpenMidiReference();
    };
    addAndMakeVisible(midiReferenceTrackButton);
    midiReferenceTrackButton.onClick = [this] { showMidiReferenceTrackMenu(); };
    addAndMakeVisible(midiReferenceVisibilityToggle);
    midiReferenceVisibilityToggle.setClickingTogglesState(true);
    midiReferenceVisibilityToggle.onClick = [this]
    {
        if (!project)
            return;
        auto state = project->getMidiReference();
        state.visible = midiReferenceVisibilityToggle.getToggleState();
        editMidiReference(std::move(state));
    };

    // MIDI button: drag the notes straight into the DAW, or click to save.
    addAndMakeVisible(midiDragButton);
    midiDragButton.setEnabled(false);
    midiDragButton.onCreateDragFile = [this]
    {
        return onCreateMidiDragFile ? onCreateMidiDragFile() : juce::File{};
    };
    midiDragButton.onClick = [this]
    {
        if (onExportMidiClicked)
            onExportMidiClicked();
    };

    addAndMakeVisible(settingsButton);
    settingsButton.onClick = [this]
    {
        if (onShowSettingsClicked)
            onShowSettingsClicked();
    };

    for (auto* toggle : { &snapToSemitonesToggle, &timelineSnapCycleToggle })
    {
        addAndMakeVisible(toggle);
        toggle->setClickingTogglesState(true);
        toggle->addListener(this);
    }

    addAndMakeVisible(pitchModeControl);
    pitchModeControl.onChange = [this](int index)
    {
        if (isUpdating)
            return;
        if (index == 0)
        {
            setScaleModeInternal(ScaleMode::Chromatic, true);
        }
        else
        {
            ScaleMode target = selectedScaleMode;
            if (target == ScaleMode::None || target == ScaleMode::Chromatic)
                target = (lastNonChromaticMode == ScaleMode::None ||
                          lastNonChromaticMode == ScaleMode::Chromatic)
                             ? ScaleMode::Major
                             : lastNonChromaticMode;
            if (selectedScaleRootNote < 0)
                setScaleRootInternal(0, true);
            setScaleModeInternal(target, true);
        }
        if (onParameterChanged)
            onParameterChanged();
    };

    addAndMakeVisible(timelineModeControl);
    timelineModeControl.onChange = [this](int index)
    {
        if (!isUpdating)
            setTimelineDisplayModeInternal(
                index == 0 ? TimelineDisplayMode::Beats : TimelineDisplayMode::Time, true);
    };

    addAndMakeVisible(renderingControl);
    renderingControl.onChange = [this](int index)
    {
        if (!isUpdating)
            setSynthesisEngineInternal(
                index == 0 ? SynthesisEngineType::Vocoder : SynthesisEngineType::Psola, true);
    };

    // Device row: added hidden. It appears only once the host tells us the
    // engine is AI Resynthesis and there is more than one device to run it on
    // (see setRenderDeviceOptions).
    addChildComponent(renderDeviceLabel);
    renderDeviceLabel.setColour(juce::Label::textColourId, APP_COLOR_PANEL_LABEL);
    renderDeviceLabel.setFont(AppFont::getFont(14.0f));
    addChildComponent(renderDeviceButton);
    renderDeviceButton.addListener(this);

    for (auto* label : { &referenceLabel, &timelineBeatLabel,
                         &timelineTempoLabel, &timelineGridLabel })
    {
        addAndMakeVisible(label);
        label->setColour(juce::Label::textColourId, APP_COLOR_TEXT_MUTED);
    }

    for (auto* label : { &referenceLabel, &timelineBeatLabel,
                         &timelineTempoLabel, &timelineGridLabel })
    {
        label->setColour(juce::Label::textColourId, APP_COLOR_PANEL_LABEL);
        label->setFont(AppFont::getFont(14.0f));
    }

    const std::array<juce::TextButton*, 3> textButtons {{
        &dragSnapModeButton, &timelineBeatButton, &timelineGridButton
    }};
    for (auto* button : textButtons)
    {
        setupTextButton(*button);
        addAndMakeVisible(button);
        button->addListener(this);
    }

    referenceSlider.setRange(420.0, 450.0, 1.0);
    referenceSlider.setValue(pitchReferenceHz, juce::dontSendNotification);
    referenceSlider.setNumDecimalPlacesToDisplay(0);
    referenceSlider.setTextValueSuffix(" Hz");
    referenceSlider.setHideSuffixWhileEditing(true);
    referenceSlider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, 64, 22);
    referenceSlider.setColour(juce::Slider::backgroundColourId,
                              APP_COLOR_CONTROL_FILL);
    referenceSlider.setColour(juce::Slider::trackColourId,
                              APP_COLOR_CONTROL_FILL);
    referenceSlider.setColour(juce::Slider::textBoxBackgroundColourId,
                              juce::Colours::transparentBlack);
    referenceSlider.setColour(juce::Slider::textBoxOutlineColourId,
                              juce::Colours::transparentBlack);
    referenceSlider.setColour(juce::Slider::textBoxTextColourId,
                              APP_COLOR_CONTROL_TEXT);
    referenceSlider.onValueChange = [this]()
    {
        if (!isUpdating)
            setPitchReferenceInternal(
                static_cast<int>(std::round(referenceSlider.getValue())), true);
    };
    addAndMakeVisible(referenceSlider);

    timelineTempoSlider.setRange(20.0, 300.0, 0.01);
    timelineTempoSlider.setValue(timelineTempoBpm, juce::dontSendNotification);
    timelineTempoSlider.setNumDecimalPlacesToDisplay(2);
    timelineTempoSlider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, 54, 22);
    timelineTempoSlider.setColour(juce::Slider::backgroundColourId,
                                  APP_COLOR_CONTROL_FILL);
    timelineTempoSlider.setColour(juce::Slider::trackColourId,
                                  APP_COLOR_CONTROL_FILL);
    timelineTempoSlider.setColour(juce::Slider::textBoxBackgroundColourId,
                                  juce::Colours::transparentBlack);
    timelineTempoSlider.setColour(juce::Slider::textBoxOutlineColourId,
                                  juce::Colours::transparentBlack);
    timelineTempoSlider.setColour(juce::Slider::textBoxTextColourId,
                                  APP_COLOR_CONTROL_TEXT);
    timelineTempoSlider.onValueChange = [this]()
    {
        if (!isUpdating)
            setTimelineTempoBpmInternal(timelineTempoSlider.getValue(), true);
    };
    addAndMakeVisible(timelineTempoSlider);

    timelineBeatButton.setButtonText(getTimelineBeatLabel(timelineBeatNumerator, timelineBeatDenominator));
    timelineGridButton.setButtonText(getTimelineGridLabel(timelineGridDivision));
    dragSnapModeButton.setButtonText(getDragSnapModeLabel(dragSnapMode));
    dragSnapModeButton.setEnabled(snapToSemitones);
    timelineSnapCycleToggle.setToggleState(timelineSnapCycle, juce::dontSendNotification);

    const std::array<juce::Component*, 8> pitchComponents {{
        &pitchSectionLabel, &pitchModeControl,
        &referenceLabel, &referenceSlider,
        &snapToSemitonesToggle, &dragSnapModeButton, &doubleClickSnapLabel, &doubleClickSnapBox
    }};
    for (auto* component : pitchComponents)
        component->setVisible(kShowPitchCard);

    refreshModeToggles();
    refreshTimelineModeToggles();
    refreshSynthesisToggles();

    // Every caption above is a translated string, applied in one place.
    refreshLocalisedText();
    startTimer(150);
}

ParameterPanel::~ParameterPanel()
{
}

void ParameterPanel::setupTextButton(juce::TextButton& button)
{
    button.setColour(juce::TextButton::buttonColourId, APP_COLOR_SURFACE_ALT);
    button.setColour(juce::TextButton::buttonOnColourId, APP_COLOR_SURFACE_RAISED);
    button.setColour(juce::TextButton::textColourOffId, APP_COLOR_TEXT_PRIMARY);
    button.setColour(juce::TextButton::textColourOnId, APP_COLOR_TEXT_PRIMARY);
}

void ParameterPanel::paint(juce::Graphics& g)
{
    // Two separate cards for Pitch and Time sections
    const float radius = 8.0f;

    if (!noteCardBounds.isEmpty())
    {
        const auto rect = noteCardBounds.toFloat();
        g.setColour(APP_COLOR_PANEL_CARD);
        g.fillRoundedRectangle(rect, radius);
        g.setColour(APP_COLOR_BORDER.withAlpha(0.4f));
        g.drawRoundedRectangle(rect.reduced(0.5f), radius, 0.75f);
    }

    if (!pitchCardBounds.isEmpty())
    {
        auto pitchRect = pitchCardBounds.toFloat();
        g.setColour(APP_COLOR_PANEL_CARD);
        g.fillRoundedRectangle(pitchRect, radius);
        g.setColour(APP_COLOR_BORDER.withAlpha(0.4f));
        g.drawRoundedRectangle(pitchRect.reduced(0.5f), radius, 0.75f);
    }

    if (!timeCardBounds.isEmpty())
    {
        auto timeRect = timeCardBounds.toFloat();
        g.setColour(APP_COLOR_PANEL_CARD);
        g.fillRoundedRectangle(timeRect, radius);
        g.setColour(APP_COLOR_BORDER.withAlpha(0.4f));
        g.drawRoundedRectangle(timeRect.reduced(0.5f), radius, 0.75f);
    }

    if (!synthesisCardBounds.isEmpty())
    {
        auto synthesisRect = synthesisCardBounds.toFloat();
        g.setColour(APP_COLOR_PANEL_CARD);
        g.fillRoundedRectangle(synthesisRect, radius);
        g.setColour(APP_COLOR_BORDER.withAlpha(0.4f));
        g.drawRoundedRectangle(synthesisRect.reduced(0.5f), radius, 0.75f);
    }

    if (!midiReferenceCardBounds.isEmpty())
    {
        const auto rect = midiReferenceCardBounds.toFloat();
        g.setColour(APP_COLOR_PANEL_CARD);
        g.fillRoundedRectangle(rect, radius);
        g.setColour(APP_COLOR_BORDER.withAlpha(0.4f));
        g.drawRoundedRectangle(rect.reduced(0.5f), radius, 0.75f);
    }
}

void ParameterPanel::resized()
{
    auto outerBounds = getLocalBounds();

    // Metrics live at the top of this file so getPreferredHeight() can sum them.
    constexpr int cardPadY = kCardPadY;
    constexpr int cardGap = kCardGap;
    constexpr int innerPadX = kInnerPadX;
    constexpr int innerPadY = kInnerPadY;
    constexpr int rowGap = kRowGap;
    constexpr int columnGap = kColumnGap;

    // Label columns are as wide as their text, so translations are never cut off.
    auto textWidth = [](const juce::Label& label)
    {
        return juce::GlyphArrangement::getStringWidthInt(label.getFont(), label.getText())
               + label.getBorderSize().getLeftAndRight() + 6;
    };

    auto cardArea = outerBounds.reduced(kCardPadX, cardPadY);
    // Cards stack top down in this order; each starts where the last ended.
    int nextCardTop = cardArea.getY();
    juce::Rectangle<int> bounds;

    // =========================================================================
    // SELECTED NOTE CARD
    // =========================================================================
    noteCardBounds = {};
    if (noteCardVisible)
    {
        noteCardBounds = { cardArea.getX(), nextCardTop, cardArea.getWidth(), kNoteCardHeight };
        bounds = noteCardBounds.reduced(innerPadX, innerPadY);
        noteSectionLabel.setBounds(bounds.removeFromTop(kSectionLabelHeight));
        bounds.removeFromTop(kSectionLabelGap);

        const int noteLabelWidth = juce::jlimit(
            60, juce::jmax(60, bounds.getWidth() - 120),
            std::max({ textWidth(noteEngineLabel), textWidth(noteDeviationLabel),
                         textWidth(noteVolumeLabel), textWidth(noteFormantLabel),
                         textWidth(noteAlignLabel), textWidth(notePresetLabel) }));
        auto engineRow = bounds.removeFromTop(kSegmentedRowHeight);
        noteEngineLabel.setBounds(engineRow.removeFromLeft(noteLabelWidth));
        noteEngineControl.setBounds(engineRow);

        const auto layoutRow = [&](juce::Label& label, juce::Component& control)
        {
            bounds.removeFromTop(rowGap + 1);
            auto row = bounds.removeFromTop(kControlRowHeight);
            label.setBounds(row.removeFromLeft(noteLabelWidth));
            control.setBounds(row.reduced(0, 2));
        };
        layoutRow(noteDeviationLabel, noteDeviationValue);
        layoutRow(noteVolumeLabel, noteVolumeSlider);
        layoutRow(noteFormantLabel, noteFormantSlider);
        layoutRow(noteAlignLabel, noteAlignSlider);
        bounds.removeFromTop(rowGap + 1);
        noteAlignButton.setBounds(bounds.removeFromTop(kControlRowHeight).reduced(0, 2));
        layoutRow(notePresetLabel, notePresetButton);
        bounds.removeFromTop(rowGap + 1);
        auto presetActions = bounds.removeFromTop(kControlRowHeight).reduced(0, 2);
        const int actionWidth = (presetActions.getWidth() - columnGap * 2) / 3;
        notePresetApplyButton.setBounds(presetActions.removeFromLeft(actionWidth));
        presetActions.removeFromLeft(columnGap);
        notePresetSaveButton.setBounds(presetActions.removeFromLeft(actionWidth));
        presetActions.removeFromLeft(columnGap);
        notePresetDeleteButton.setBounds(presetActions);
        nextCardTop = noteCardBounds.getBottom() + cardGap;
    }

    // =========================================================================
    // PITCH CARD
    // =========================================================================
    const int pitchCardStart = nextCardTop;
    bounds = juce::Rectangle<int>(cardArea.getX() + innerPadX, pitchCardStart + innerPadY,
                                  cardArea.getWidth() - innerPadX * 2,
                                  cardArea.getBottom() - pitchCardStart - innerPadY * 2);

    pitchSectionLabel.setBounds(bounds.removeFromTop(kSectionLabelHeight));
    bounds.removeFromTop(kSectionLabelGap);

    // Mode toggles: Chromatic | Scale
    pitchModeControl.setBounds(bounds.removeFromTop(kSegmentedRowHeight));

    bounds.removeFromTop(rowGap + 1);

    // The drag-snap toggle draws its switch (38px with gap) before the text.
    const int toggleTextWidth = 38 + juce::GlyphArrangement::getStringWidthInt(
        AppFont::getFont(14.0f), snapToSemitonesToggle.getButtonText()) + 6;
    const int pitchLabelWidth = juce::jlimit(
        80, juce::jmax(80, bounds.getWidth() - 96),
        juce::jmax(textWidth(referenceLabel), textWidth(doubleClickSnapLabel), toggleTextWidth));
    constexpr int pitchControlOffsetX = 1;

    // Reference row
    auto referenceRow = bounds.removeFromTop(kControlRowHeight);
    referenceLabel.setBounds(referenceRow.removeFromLeft(pitchLabelWidth));
    referenceSlider.setBounds(referenceRow.translated(pitchControlOffsetX, 0).reduced(0, 2));

    bounds.removeFromTop(rowGap + 1);
    auto dragSnapRow = bounds.removeFromTop(kControlRowHeight);
    snapToSemitonesToggle.setBounds(dragSnapRow.removeFromLeft(pitchLabelWidth));
    dragSnapModeButton.setBounds(dragSnapRow.translated(pitchControlOffsetX, 0));

    bounds.removeFromTop(rowGap + 1);
    auto doubleClickRow = bounds.removeFromTop(kControlRowHeight);
    doubleClickSnapLabel.setBounds(doubleClickRow.removeFromLeft(pitchLabelWidth));
    doubleClickSnapBox.setBounds(doubleClickRow.translated(pitchControlOffsetX, 0));

    const int pitchCardBottom = bounds.getY() + innerPadY;
    pitchCardBounds = kShowPitchCard
        ? juce::Rectangle<int>(cardArea.getX(), pitchCardStart,
                               cardArea.getWidth(), pitchCardBottom - pitchCardStart)
        : juce::Rectangle<int>();

    nextCardTop = pitchCardBottom + cardGap;

    // =========================================================================
    // TIME CARD
    // =========================================================================
    const int timeCardStart = nextCardTop;
    bounds = juce::Rectangle<int>(cardArea.getX() + innerPadX, timeCardStart + innerPadY,
                                   cardArea.getWidth() - innerPadX * 2, cardArea.getBottom() - timeCardStart - innerPadY * 2);

    timeSectionLabel.setBounds(bounds.removeFromTop(kSectionLabelHeight));
    bounds.removeFromTop(kSectionLabelGap);

    // Timeline mode: Beats | Time
    timelineModeControl.setBounds(bounds.removeFromTop(kSegmentedRowHeight));

    bounds.removeFromTop(rowGap + 1);

    // Beat / Tempo row
    auto beatTempoRow = bounds.removeFromTop(kControlRowHeight);
    const int timeLabelWidth = juce::jmax(textWidth(timelineBeatLabel), textWidth(timelineGridLabel));
    timelineBeatLabel.setBounds(beatTempoRow.removeFromLeft(timeLabelWidth));
    auto beatButtonArea = beatTempoRow.removeFromLeft(44);
    timelineBeatButton.setBounds(beatButtonArea.withSizeKeepingCentre(40, 22));
    beatTempoRow.removeFromLeft(10);
    timelineTempoLabel.setBounds(beatTempoRow.removeFromLeft(textWidth(timelineTempoLabel)));
    timelineTempoSlider.setBounds(beatTempoRow.reduced(0, 2));

    bounds.removeFromTop(rowGap);

    // Grid / Snap Cycle row
    auto gridRow = bounds.removeFromTop(kControlRowHeight);
    timelineGridLabel.setBounds(gridRow.removeFromLeft(timeLabelWidth));
    timelineGridButton.setBounds(
        gridRow.removeFromLeft(44).withSizeKeepingCentre(40, 22));

    bounds.removeFromTop(rowGap);
    timelineSnapCycleToggle.setBounds(bounds.removeFromTop(kControlRowHeight));

    int timeCardBottom = bounds.getY() + innerPadY;
    timeCardBounds = juce::Rectangle<int>(cardArea.getX(), timeCardStart,
                                           cardArea.getWidth(), timeCardBottom - timeCardStart);

    nextCardTop = timeCardBottom + cardGap;

    // =========================================================================
    // SYNTHESIS CARD
    // =========================================================================
    const int synthesisCardStart = nextCardTop;
    bounds = juce::Rectangle<int>(cardArea.getX() + innerPadX,
                                       synthesisCardStart + innerPadY,
                                       cardArea.getWidth() - innerPadX * 2,
                                       cardArea.getBottom() - synthesisCardStart - innerPadY * 2);

    synthesisSectionLabel.setBounds(bounds.removeFromTop(kSectionLabelHeight));
    bounds.removeFromTop(kSectionLabelGap);

    renderingControl.setBounds(bounds.removeFromTop(kSegmentedRowHeight));

    if (renderDeviceRowVisible)
    {
        bounds.removeFromTop(rowGap + 1);
        auto deviceRow = bounds.removeFromTop(kControlRowHeight);
        renderDeviceLabel.setBounds(deviceRow.removeFromLeft(52));
        renderDeviceButton.setBounds(deviceRow.reduced(0, 2));
    }

    const int synthesisCardBottom = bounds.getY() + innerPadY;
    synthesisCardBounds = juce::Rectangle<int>(
        cardArea.getX(), synthesisCardStart, cardArea.getWidth(),
        synthesisCardBottom - synthesisCardStart);

    nextCardTop = synthesisCardBottom + cardGap;

    const int midiReferenceTop = nextCardTop;
    midiReferenceCardBounds = {cardArea.getX(), midiReferenceTop,
                               cardArea.getWidth(), kMidiReferenceCardHeight};
    bounds = midiReferenceCardBounds.reduced(innerPadX, innerPadY);
    midiReferenceSectionLabel.setBounds(bounds.removeFromTop(kSectionLabelHeight));
    bounds.removeFromTop(kSectionLabelGap);

    midiDragButton.setBounds(bounds.removeFromTop(kMidiButtonHeight));
    bounds.removeFromTop(kMidiExportRowGap);

    auto midiFileRow = bounds.removeFromTop(kMidiButtonHeight);
    const int importWidth = (midiFileRow.getWidth() - columnGap) * 3 / 5;
    midiReferenceFileButton.setBounds(midiFileRow.removeFromLeft(importWidth));
    midiFileRow.removeFromLeft(columnGap);
    midiReferenceTrackButton.setBounds(midiFileRow);

    bounds.removeFromTop(rowGap);
    midiReferenceVisibilityToggle.setBounds(bounds.removeFromTop(kControlRowHeight));
    nextCardTop = midiReferenceCardBounds.getBottom() + cardGap;

    settingsButton.setBounds(cardArea.getX(), nextCardTop, cardArea.getWidth(), kMidiButtonHeight);
    const int stackBottom = settingsButton.getBottom();

    // The stack is laid out at fixed sizes, so given at least the height it
    // asks for its bottom edge must land exactly where getPreferredHeight()
    // says it will - that value is what the hosting panel scrolls against. A
    // shorter panel is not checked: removeFromTop clamps once the space runs
    // out, which is the very case the scrolling exists to avoid.
    const int preferredHeight = getPreferredHeight();
    jassert(outerBounds.getHeight() < preferredHeight ||
            stackBottom + cardPadY - outerBounds.getY() ==
                preferredHeight);
    juce::ignoreUnused(preferredHeight);
}

int ParameterPanel::getPreferredHeight() const
{
    return kPreferredPanelHeight +
           (renderDeviceRowVisible ? kRenderDeviceRowExtraHeight : 0) +
           (noteCardVisible ? kNoteCardHeight + kCardGap : 0);
}

void ParameterPanel::buttonClicked(juce::Button* button)
{
    if (isUpdating)
        return;

    if (button == &timelineBeatButton)
    {
        showTimelineBeatMenu();
        return;
    }
    if (button == &timelineGridButton)
    {
        showTimelineGridMenu();
        return;
    }
    if (button == &dragSnapModeButton)
    {
        showDragSnapModeMenu();
        return;
    }
    if (button == &renderDeviceButton)
    {
        showRenderDeviceMenu();
        return;
    }
    if (button == &timelineSnapCycleToggle)
    {
        setTimelineSnapCycleInternal(timelineSnapCycleToggle.getToggleState(), true);
        return;
    }
    if (button == &snapToSemitonesToggle)
    {
        setSnapToSemitonesInternal(snapToSemitonesToggle.getToggleState(), true);
        if (onParameterChanged)
            onParameterChanged();
    }
}

void ParameterPanel::setProject(Project* proj)
{
    project = proj;
    refreshMidiReference();
    updateGlobalSliders();
    updateFromNote();
    if (onProjectBound)
        onProjectBound(project);
}

void ParameterPanel::refreshLocalisedText()
{
    settingsButton.setButtonText(TR("command.settings") + juce::String(juce::CharPointer_UTF8("\xe2\x80\xa6")));

    noteSectionLabel.setText(TR("panel.selected_note"), juce::dontSendNotification);
    noteEngineLabel.setText(TR("param.engine"), juce::dontSendNotification);
    noteEngineControl.setOptions({ TR("engine.default_short"), TR("engine.ai_short"),
                                   TR("param.engine_psola") });
    noteEngineControl.setOptionTooltip(0, TR("engine.default"));
    noteEngineControl.setOptionTooltip(1, TR("param.engine_vocoder"));
    noteDeviationLabel.setText(TR("param.deviation"), juce::dontSendNotification);
    noteVolumeLabel.setText(TR("param.volume"), juce::dontSendNotification);
    noteVolumeSlider.setName(TR("param.volume"));
    noteFormantLabel.setText(TR("param.formant"), juce::dontSendNotification);
    noteFormantSlider.setName(TR("param.formant"));
    noteAlignLabel.setText(TR("param.align_strength"), juce::dontSendNotification);
    noteAlignSlider.setName(TR("param.align_strength"));
    noteAlignButton.setButtonText(TR("command.align_to_reference"));
    noteAlignButton.setTooltip(TR("command.align_to_reference.desp"));
    notePresetLabel.setText(TR("param.preset"), juce::dontSendNotification);
    notePresetApplyButton.setButtonText(TR("preset.apply"));
    notePresetSaveButton.setButtonText(TR("preset.save"));
    notePresetDeleteButton.setButtonText(TR("preset.delete"));
    notePresetButton.setTooltip(TR("preset.tooltip"));
    refreshPresetControls();

    pitchSectionLabel.setText(TR("panel.pitch"), juce::dontSendNotification);
    timeSectionLabel.setText(TR("panel.time"), juce::dontSendNotification);
    synthesisSectionLabel.setText(TR("panel.rendering"), juce::dontSendNotification);
    midiDragButton.setButtonText(TR("param.midi_drag"));
    midiDragButton.setTooltip(TR("tooltip.midi_drag"));
    midiReferenceSectionLabel.setText(TR("panel.midi"), juce::dontSendNotification);
    midiReferenceVisibilityToggle.setButtonText(TR("param.show_midi_reference"));
    refreshMidiReferenceControls();

    pitchModeControl.setOptions({ TR("param.chromatic"), TR("param.scale") });

    // The option text stays plain language; the algorithm names live in the
    // tooltips, so they are discoverable without crowding the control.
    renderingControl.setOptions({ TR("param.engine_vocoder"), TR("param.engine_psola") });
    renderingControl.setOptionTooltip(0, TR(aiResynthesisAvailable ? "tooltip.engine_vocoder"
                                                                   : "tooltip.engine_vocoder_unavailable"));
    renderingControl.setOptionTooltip(1, TR("tooltip.engine_psola"));

    renderDeviceLabel.setText(TR("param.device"), juce::dontSendNotification);

    referenceLabel.setText(TR("param.reference_a4"), juce::dontSendNotification);
    referenceSlider.setName(TR("param.pitch_reference"));

    snapToSemitonesToggle.setButtonText(TR("param.drag_snap"));

    timelineModeControl.setOptions({ TR("param.beats"), TR("param.time_mode") });
    timelineBeatLabel.setText(TR("param.beat"), juce::dontSendNotification);
    timelineTempoLabel.setText(TR("param.tempo"), juce::dontSendNotification);
    timelineTempoSlider.setName(TR("param.tempo"));
    timelineGridLabel.setText(TR("param.grid"), juce::dontSendNotification);
    timelineSnapCycleToggle.setButtonText(TR("param.snap_cycle"));

    // Values whose text is itself a translated word.
    dragSnapModeButton.setButtonText(getDragSnapModeLabel(dragSnapMode));
    const auto doubleClickSnapText = TR("param.double_click_snap");
    doubleClickSnapLabel.setText(doubleClickSnapText, juce::dontSendNotification);
    doubleClickSnapBox.setTitle(doubleClickSnapText);
    doubleClickSnapBox.setButtonText(getDoubleClickSnapModeLabel(
        project ? project->getDoubleClickSnapMode() : DoubleClickSnapMode::PitchCenter));
    refreshRenderDeviceRow();
    if (!getLocalBounds().isEmpty())
        resized();
}

void ParameterPanel::setMidiReference(juce::String filename,
                                      std::vector<MidiReference::Track> tracks)
{
    auto guide = std::make_shared<MidiReference::Guide>(
        MidiReference::Guide{ std::move(filename), std::move(tracks) });
    editMidiReference({ std::move(guide), 0, true });
}

void ParameterPanel::editMidiReference(MidiReference::State state)
{
    if (!project || !project->setMidiReference(std::move(state)))
        return;

    refreshMidiReference();
    if (onMidiReferenceEdited)
        onMidiReferenceEdited();
}

void ParameterPanel::refreshMidiReferenceControls()
{
    const auto state = project ? project->getMidiReference() : MidiReference::State{};
    const bool loaded = state.guide != nullptr;
    midiReferenceFileButton.setEnabled(project != nullptr);
    midiReferenceFileButton.setButtonText(
        loaded ? state.guide->filename : TR("param.midi_reference_import"));
    midiReferenceFileButton.setTooltip(midiReferenceFileButton.getButtonText());
    midiReferenceTrackButton.setEnabled(loaded);
    midiReferenceVisibilityToggle.setEnabled(loaded);
    midiReferenceVisibilityToggle.setToggleState(state.visible, juce::dontSendNotification);

    const auto trackName = loaded
        ? getMidiReferenceTrackLabel(state.guide->tracks[state.selectedTrack])
        : juce::String{};
    midiReferenceTrackButton.setButtonText(trackName);
    midiReferenceTrackButton.setTooltip(trackName);
}

void ParameterPanel::refreshMidiReference()
{
    ++midiReferenceRevision;
    refreshMidiReferenceControls();
    if (!onMidiReferenceChanged)
        return;

    const auto state = project ? project->getMidiReference() : MidiReference::State{};
    if (state.guide)
        onMidiReferenceChanged(state.guide->tracks[state.selectedTrack].notes, state.visible);
    else
        onMidiReferenceChanged({}, false);
}

void ParameterPanel::showMidiReferenceTrackMenu()
{
    const auto state = project ? project->getMidiReference() : MidiReference::State{};
    if (!state.guide)
        return;

    juce::PopupMenu menu;
    menu.setLookAndFeel(&getPitchPopupLookAndFeel());
    for (size_t index = 0; index < state.guide->tracks.size(); ++index)
    {
        const auto label = getMidiReferenceTrackLabel(state.guide->tracks[index]);
        menu.addCustomItem(static_cast<int>(index) + 1,
                           std::make_unique<HoverMenuItemComponent>(
                               label, index == state.selectedTrack),
                           nullptr, label);
    }
    menu.showMenuAsync(juce::PopupMenu::Options()
                           .withTargetComponent(&midiReferenceTrackButton)
                           .withMinimumWidth(midiReferenceTrackButton.getWidth()),
        [safeThis = juce::Component::SafePointer<ParameterPanel>(this),
         revision = midiReferenceRevision](int result)
        {
            if (!safeThis || revision != safeThis->midiReferenceRevision || result <= 0)
                return;

            auto selection = safeThis->project->getMidiReference();
            const auto trackIndex = static_cast<size_t>(result - 1);
            if (!selection.guide || trackIndex >= selection.guide->tracks.size())
                return;

            selection.selectedTrack = trackIndex;
            safeThis->editMidiReference(std::move(selection));
        });
}

void ParameterPanel::setPluginMode(bool pluginMode)
{
    timelineBeatButton.setEnabled(!pluginMode);
    timelineTempoSlider.setEnabled(!pluginMode);
    timelineGridButton.setEnabled(true);
}

void ParameterPanel::setHostTimelineState(double bpm, int numerator, int denominator)
{
    setTimelineTempoBpmInternal(bpm, false);
    setTimelineBeatSignatureInternal(numerator, denominator, false);
}

void ParameterPanel::setSelectedNote(Note* note)
{
    selectedNote = note;
    updateFromNote();
}

void ParameterPanel::updateFromNote()
{
    midiDragButton.setEnabled(project != nullptr &&
                              MidiExporter::hasExportableNotes(project->getNotes()));
    refreshNoteCard();
}

void ParameterPanel::timerCallback()
{
    // Selection changes from many places in the piano roll (clicks, box select,
    // keys, undo) without one shared callback, so the card polls it.
    if (isShowing())
        refreshNoteCard();
}

void ParameterPanel::refreshNoteCard()
{
    std::vector<const Note*> notes;
    if (project != nullptr)
        for (const auto* note : project->getSelectedNotes())
            if (!note->isRest())
                notes.push_back(note);

    const bool show = !notes.empty();
    if (show != noteCardVisible)
    {
        noteCardVisible = show;
        for (auto* component : std::initializer_list<juce::Component*>{
                 &noteSectionLabel, &noteEngineLabel, &noteEngineControl, &noteDeviationLabel,
                 &noteDeviationValue, &noteVolumeLabel, &noteVolumeSlider, &noteFormantLabel,
                 &noteFormantSlider, &noteAlignLabel, &noteAlignSlider, &noteAlignButton,
                 &notePresetLabel, &notePresetButton, &notePresetApplyButton,
                 &notePresetSaveButton, &notePresetDeleteButton })
            component->setVisible(show);
        resized();
        repaint();
        if (onPreferredHeightChanged)
            onPreferredHeightChanged();
    }
    if (!show)
        return;

    const auto* first = notes.front();
    const bool sharedEngine = std::all_of(notes.begin(), notes.end(), [first](const Note* note)
                                          { return note->getEngine() == first->getEngine(); });
    noteEngineControl.setSelectedIndex(sharedEngine ? static_cast<int>(first->getEngine()) : -1);

    int lowest = std::numeric_limits<int>::max(), highest = std::numeric_limits<int>::min();
    for (const auto* note : notes)
    {
        const int cents = juce::roundToInt(NoteDeviation::cents(*project, *note));
        lowest = std::min(lowest, cents);
        highest = std::max(highest, cents);
    }
    const auto formatCents = [](int cents) { return (cents > 0 ? "+" : "") + juce::String(cents); };
    noteDeviationValue.setText(lowest == highest
                                   ? formatCents(lowest) + " ct"
                                   : formatCents(lowest) + " ~ " + formatCents(highest) + " ct",
                               juce::dontSendNotification);

    // shortcut: a mixed selection shows the first note's level and formant, and
    // an edit sets them all to that value; show a range if users ask for one.
    if (!noteSliderDragging)
    {
        noteVolumeSlider.setValue(first->getVolumeDb(), juce::dontSendNotification);
        noteFormantSlider.setValue(first->getFormantShift(), juce::dontSendNotification);
    }
}

void ParameterPanel::setCorrectionPresetNames(const juce::StringArray& names,
                                              const juce::String& selected)
{
    presetNames = names;
    selectedPresetName = names.contains(selected) ? selected
                         : names.contains(selectedPresetName) ? selectedPresetName
                         : names.isEmpty() ? juce::String() : names[0];
    refreshPresetControls();
}

void ParameterPanel::refreshPresetControls()
{
    notePresetButton.setButtonText(selectedPresetName.isNotEmpty() ? selectedPresetName
                                                                   : TR("preset.none"));
    notePresetButton.setEnabled(!presetNames.isEmpty());
    notePresetApplyButton.setEnabled(selectedPresetName.isNotEmpty());
    notePresetDeleteButton.setEnabled(selectedPresetName.isNotEmpty());
}

void ParameterPanel::showPresetMenu()
{
    if (presetNames.isEmpty())
        return;
    juce::PopupMenu menu;
    menu.setLookAndFeel(&getPitchPopupLookAndFeel());
    for (int i = 0; i < presetNames.size(); ++i)
        menu.addCustomItem(i + 1,
                           std::make_unique<HoverMenuItemComponent>(
                               presetNames[i], presetNames[i] == selectedPresetName),
                           nullptr, presetNames[i]);
    menu.showMenuAsync(
        juce::PopupMenu::Options()
            .withTargetComponent(&notePresetButton)
            .withParentComponent(this)
            .withMinimumWidth(notePresetButton.getWidth()),
        [safeThis = juce::Component::SafePointer<ParameterPanel>(this)](int result)
        {
            if (safeThis == nullptr || result <= 0 || result > safeThis->presetNames.size())
                return;
            safeThis->selectedPresetName = safeThis->presetNames[result - 1];
            safeThis->refreshPresetControls();
        });
}

void ParameterPanel::askPresetName()
{
    auto* window = new juce::AlertWindow(TR("preset.save_title"), TR("preset.save_message"),
                                         juce::MessageBoxIconType::NoIcon, this);
    window->addTextEditor("name", selectedPresetName);
    window->addButton(TR("dialog.ok"), 1, juce::KeyPress(juce::KeyPress::returnKey));
    window->addButton(TR("dialog.cancel"), 0, juce::KeyPress(juce::KeyPress::escapeKey));
    window->enterModalState(
        true,
        juce::ModalCallbackFunction::create(
            [safeThis = juce::Component::SafePointer<ParameterPanel>(this), window](int result)
            {
                const auto name = window->getTextEditorContents("name").trim();
                if (safeThis == nullptr || result != 1 || name.isEmpty())
                    return;
                safeThis->selectedPresetName = name;
                if (safeThis->onSaveCorrectionPreset)
                    safeThis->onSaveCorrectionPreset(name);
            }),
        true);
}

void ParameterPanel::updateGlobalSliders()
{
    isUpdating = true;
    doubleClickSnapBox.setEnabled(project != nullptr);
    doubleClickSnapBox.setButtonText(getDoubleClickSnapModeLabel(
        project ? project->getDoubleClickSnapMode() : DoubleClickSnapMode::PitchCenter));
    lastNonChromaticMode = ScaleMode::Major;

    if (project != nullptr)
    {
        selectedScaleRootNote = project->getScaleRootNote();
        if (selectedScaleRootNote < 0)
        {
            selectedScaleRootNote = 0;
            project->setScaleRootNote(selectedScaleRootNote);
        }
        selectedScaleMode = project->getScaleMode();
        if (selectedScaleMode == ScaleMode::None)
        {
            selectedScaleMode = ScaleMode::Chromatic;
            project->setScaleMode(selectedScaleMode);
        }
        lastNonChromaticMode = project->getPreferredScaleMode();
        snapToSemitones = project->getSnapToSemitones();
        dragSnapMode = project->getDragSnapMode();
        pitchReferenceHz = project->getPitchReferenceHz();
        timelineDisplayMode = project->getTimelineDisplayMode();
        timelineBeatNumerator = project->getTimelineBeatNumerator();
        timelineBeatDenominator = project->getTimelineBeatDenominator();
        timelineTempoBpm = project->getTimelineTempoBpm();
        timelineGridDivision = project->getTimelineGridDivision();
        timelineSnapCycle = project->getTimelineSnapCycle();
    }
    else
    {
        selectedScaleRootNote = 0;
        selectedScaleMode = ScaleMode::Chromatic;
        snapToSemitones = false;
        dragSnapMode = DragSnapMode::Chromatic;
        pitchReferenceHz = 440;
        timelineDisplayMode = TimelineDisplayMode::Beats;
        timelineBeatNumerator = 4;
        timelineBeatDenominator = 4;
        timelineTempoBpm = 120.0;
        timelineGridDivision = TimelineGridDivision::Quarter;
        timelineSnapCycle = false;
    }

    referenceSlider.setValue(pitchReferenceHz, juce::dontSendNotification);
    snapToSemitonesToggle.setToggleState(snapToSemitones, juce::dontSendNotification);
    dragSnapModeButton.setButtonText(getDragSnapModeLabel(dragSnapMode));
    dragSnapModeButton.setEnabled(snapToSemitones);
    timelineBeatButton.setButtonText(
        getTimelineBeatLabel(timelineBeatNumerator, timelineBeatDenominator));
    timelineTempoSlider.setValue(timelineTempoBpm, juce::dontSendNotification);
    timelineGridButton.setButtonText(getTimelineGridLabel(timelineGridDivision));
    timelineSnapCycleToggle.setToggleState(timelineSnapCycle, juce::dontSendNotification);

    refreshModeToggles();
    refreshTimelineModeToggles();
    isUpdating = false;
}

void ParameterPanel::refreshModeToggles()
{
    pitchModeControl.setSelectedIndex(
        selectedScaleMode == ScaleMode::Chromatic ? 0
        : selectedScaleMode == ScaleMode::None    ? -1
                                                  : 1);
}

void ParameterPanel::setSynthesisEngine(SynthesisEngineType type)
{
    setSynthesisEngineInternal(type, false);
}

void ParameterPanel::setSynthesisEngineInternal(SynthesisEngineType type, bool notify)
{
    synthesisEngine = type;
    refreshSynthesisToggles();

    if (notify && onSynthesisEngineChanged)
        onSynthesisEngineChanged(synthesisEngine);
}

void ParameterPanel::refreshSynthesisToggles()
{
    renderingControl.setSelectedIndex(synthesisEngine == SynthesisEngineType::Vocoder ? 0 : 1);
    refreshRenderDeviceRow();
}

void ParameterPanel::setAiResynthesisAvailable(bool available)
{
    aiResynthesisAvailable = available;
    renderingControl.setOptionEnabled(0, available);
    renderingControl.setOptionTooltip(0, TR(available ? "tooltip.engine_vocoder"
                                                      : "tooltip.engine_vocoder_unavailable"));
}

void ParameterPanel::setRenderDeviceOptions(const juce::StringArray& deviceNames,
                                            int selectedIndex)
{
    renderDeviceNames = deviceNames;
    renderDeviceIndex = juce::jlimit(0, juce::jmax(0, renderDeviceNames.size() - 1),
                                     selectedIndex);
    refreshRenderDeviceRow();
}

void ParameterPanel::refreshRenderDeviceRow()
{
    // Only PSOLA's counterpart runs a model, and a single-device machine has
    // nothing to pick between, so anything else leaves the row out entirely.
    const bool shouldShow = synthesisEngine == SynthesisEngineType::Vocoder &&
                            renderDeviceNames.size() > 1;

    renderDeviceButton.setButtonText(
        juce::isPositiveAndBelow(renderDeviceIndex, renderDeviceNames.size())
            ? getRenderDeviceLabel(renderDeviceNames[renderDeviceIndex])
            : TR("param.default_device"));

    if (shouldShow == renderDeviceRowVisible)
    {
        renderDeviceButton.repaint();
        return;
    }

    renderDeviceRowVisible = shouldShow;
    renderDeviceLabel.setVisible(shouldShow);
    renderDeviceButton.setVisible(shouldShow);

    resized();
    repaint();

    // The Rendering card just grew or shrank, and the hosting panel caches the
    // stack height to decide when to scroll.
    if (onPreferredHeightChanged)
        onPreferredHeightChanged();
}

void ParameterPanel::showRenderDeviceMenu()
{
    if (renderDeviceNames.size() < 2)
        return;

    juce::PopupMenu menu;
    menu.setLookAndFeel(&getPitchPopupLookAndFeel());
    for (int i = 0; i < renderDeviceNames.size(); ++i)
    {
        const auto label = getRenderDeviceLabel(renderDeviceNames[i]);
        menu.addCustomItem(kRenderDeviceMenuBaseId + i,
                           std::make_unique<HoverMenuItemComponent>(
                               label, i == renderDeviceIndex),
                           nullptr, label);
    }

    menu.showMenuAsync(
        juce::PopupMenu::Options()
            .withTargetComponent(&renderDeviceButton)
            .withParentComponent(this)
            .withMinimumWidth(renderDeviceButton.getWidth()),
        [safeThis = juce::Component::SafePointer<ParameterPanel>(this)](int result)
        {
            if (safeThis == nullptr || result < kRenderDeviceMenuBaseId)
                return;

            const int index = result - kRenderDeviceMenuBaseId;
            if (!juce::isPositiveAndBelow(index, safeThis->renderDeviceNames.size()))
                return;
            if (index == safeThis->renderDeviceIndex)
                return;

            safeThis->renderDeviceIndex = index;
            safeThis->refreshRenderDeviceRow();
            if (safeThis->onRenderDeviceChanged)
                safeThis->onRenderDeviceChanged(index);
        });
}

void ParameterPanel::refreshTimelineModeToggles()
{
    timelineModeControl.setSelectedIndex(timelineDisplayMode == TimelineDisplayMode::Beats ? 0 : 1);
}

void ParameterPanel::setScaleRootInternal(int rootNote, bool notify)
{
    const int normalized = juce::jlimit(-1, 11, rootNote);
    const bool changed = selectedScaleRootNote != normalized;
    if (!changed && !notify)
        return;

    selectedScaleRootNote = normalized;
    if (project != nullptr && changed)
        project->setScaleRootNote(normalized);

    if (notify && changed && onScaleRootChanged)
        onScaleRootChanged(normalized);
}

void ParameterPanel::setScaleModeInternal(ScaleMode mode, bool notify)
{
    const bool changed = selectedScaleMode != mode;
    if (!changed && !notify)
        return;

    selectedScaleMode = mode;
    if (mode != ScaleMode::None && mode != ScaleMode::Chromatic)
        lastNonChromaticMode = mode;

    refreshModeToggles();
    if (project != nullptr && changed)
        project->setScaleMode(mode);

    if (notify && changed && onScaleModeChanged)
        onScaleModeChanged(mode);
}

void ParameterPanel::setSnapToSemitonesInternal(bool enabled, bool notify)
{
    const bool changed = snapToSemitones != enabled;
    snapToSemitones = enabled;
    snapToSemitonesToggle.setToggleState(enabled, juce::dontSendNotification);
    dragSnapModeButton.setEnabled(enabled);

    if (project != nullptr && changed)
        project->setSnapToSemitones(enabled);

    if (notify && changed && onSnapToSemitonesChanged)
        onSnapToSemitonesChanged(enabled);
}

void ParameterPanel::setDragSnapModeInternal(DragSnapMode mode, bool notify)
{
    const bool changed = dragSnapMode != mode;
    dragSnapMode = mode;
    dragSnapModeButton.setButtonText(getDragSnapModeLabel(mode));

    if (project != nullptr && changed)
        project->setDragSnapMode(mode);

    if (notify && changed && onDragSnapModeChanged)
        onDragSnapModeChanged(mode);
}

void ParameterPanel::setPitchReferenceInternal(int hz, bool notify)
{
    const int normalized = juce::jlimit(420, 450, hz);
    const bool changed = pitchReferenceHz != normalized;
    pitchReferenceHz = normalized;
    referenceSlider.setValue(normalized, juce::dontSendNotification);

    if (project != nullptr && changed)
        project->setPitchReferenceHz(normalized);

    if (notify && changed && onPitchReferenceChanged)
        onPitchReferenceChanged(normalized);
}

void ParameterPanel::setTimelineDisplayModeInternal(TimelineDisplayMode mode, bool notify)
{
    const bool changed = timelineDisplayMode != mode;
    if (!changed && !notify)
        return;

    timelineDisplayMode = mode;
    refreshTimelineModeToggles();

    if (project != nullptr && changed)
        project->setTimelineDisplayMode(mode);

    if (notify && changed && onTimelineDisplayModeChanged)
        onTimelineDisplayModeChanged(mode);
}

void ParameterPanel::setTimelineBeatSignatureInternal(int numerator, int denominator, bool notify)
{
    const int normalizedNumerator = juce::jlimit(1, 32, numerator);
    const int normalizedDenominator = normalizeTimelineBeatDenominator(denominator);
    const bool changed = timelineBeatNumerator != normalizedNumerator ||
                         timelineBeatDenominator != normalizedDenominator;
    if (!changed && !notify)
        return;

    timelineBeatNumerator = normalizedNumerator;
    timelineBeatDenominator = normalizedDenominator;
    timelineBeatButton.setButtonText(
        getTimelineBeatLabel(timelineBeatNumerator, timelineBeatDenominator));

    if (project != nullptr && changed)
        project->setTimelineBeatSignature(normalizedNumerator, normalizedDenominator);

    if (notify && changed && onTimelineBeatSignatureChanged)
        onTimelineBeatSignatureChanged(normalizedNumerator, normalizedDenominator);
}

void ParameterPanel::setTimelineTempoBpmInternal(double bpm, bool notify)
{
    const double normalized = juce::jlimit(20.0, 300.0, bpm);
    const bool changed = std::abs(timelineTempoBpm - normalized) > 1.0e-6;
    timelineTempoBpm = normalized;
    timelineTempoSlider.setValue(normalized, juce::dontSendNotification);

    if (project != nullptr && changed)
        project->setTimelineTempoBpm(normalized);

    if (notify && changed && onTimelineTempoChanged)
        onTimelineTempoChanged(normalized);
}

void ParameterPanel::setTimelineGridDivisionInternal(TimelineGridDivision division, bool notify)
{
    const bool changed = timelineGridDivision != division;
    if (!changed && !notify)
        return;

    timelineGridDivision = division;
    timelineGridButton.setButtonText(getTimelineGridLabel(division));

    if (project != nullptr && changed)
        project->setTimelineGridDivision(division);

    if (notify && changed && onTimelineGridDivisionChanged)
        onTimelineGridDivisionChanged(division);
}

void ParameterPanel::setTimelineSnapCycleInternal(bool enabled, bool notify)
{
    const bool changed = timelineSnapCycle != enabled;
    timelineSnapCycle = enabled;
    timelineSnapCycleToggle.setToggleState(enabled, juce::dontSendNotification);

    if (project != nullptr && changed)
        project->setTimelineSnapCycle(enabled);

    if (notify && changed && onTimelineSnapCycleChanged)
        onTimelineSnapCycleChanged(enabled);
}

void ParameterPanel::showDragSnapModeMenu()
{
    constexpr int chromaticId = 7301;
    constexpr int scaleId = 7302;
    juce::PopupMenu menu;
    menu.setLookAndFeel(&getPitchPopupLookAndFeel());
    const auto chromaticText = TR("param.chromatic");
    const auto scaleText = TR("param.scale");
    menu.addCustomItem(
        chromaticId,
        std::make_unique<HoverMenuItemComponent>(
            chromaticText, dragSnapMode == DragSnapMode::Chromatic),
        nullptr, chromaticText);
    menu.addCustomItem(
        scaleId,
        std::make_unique<HoverMenuItemComponent>(
            scaleText, dragSnapMode == DragSnapMode::Scale),
        nullptr, scaleText);

    menu.showMenuAsync(
        juce::PopupMenu::Options()
            .withTargetComponent(&dragSnapModeButton)
            .withParentComponent(this)
            .withMinimumWidth(dragSnapModeButton.getWidth()),
        [safeThis = juce::Component::SafePointer<ParameterPanel>(this)](int result)
        {
            if (safeThis == nullptr)
                return;
            if (result == 7301)
                safeThis->setDragSnapModeInternal(
                    DragSnapMode::Chromatic, true);
            else if (result == 7302)
                safeThis->setDragSnapModeInternal(DragSnapMode::Scale, true);
        });
}

void ParameterPanel::showDoubleClickSnapModeMenu()
{
    if (project == nullptr)
        return;

    constexpr std::array modes{ DoubleClickSnapMode::PitchCenter, DoubleClickSnapMode::Scale,
                                DoubleClickSnapMode::MidiReference };
    juce::PopupMenu menu;
    menu.setLookAndFeel(&getPitchPopupLookAndFeel());
    const auto selectedMode = project->getDoubleClickSnapMode();
    for (size_t index = 0; index < modes.size(); ++index)
    {
        const auto text = getDoubleClickSnapModeLabel(modes[index]);
        menu.addCustomItem(static_cast<int>(index) + 1,
                           std::make_unique<HoverMenuItemComponent>(text, modes[index] == selectedMode),
                           nullptr, text);
    }

    menu.showMenuAsync(
        juce::PopupMenu::Options()
            .withTargetComponent(&doubleClickSnapBox)
            .withParentComponent(this)
            .withMinimumWidth(doubleClickSnapBox.getWidth()),
        [safeThis = juce::Component::SafePointer<ParameterPanel>(this),
         openedProject = project, modes](int result)
        {
            if (!safeThis || safeThis->project != openedProject ||
                !juce::isPositiveAndBelow(result - 1, static_cast<int>(modes.size())))
                return;

            const auto mode = modes[static_cast<size_t>(result - 1)];
            if (openedProject->getDoubleClickSnapMode() == mode)
                return;

            openedProject->setDoubleClickSnapMode(mode);
            safeThis->doubleClickSnapBox.setButtonText(getDoubleClickSnapModeLabel(mode));
            if (safeThis->onDoubleClickSnapModeChanged)
                safeThis->onDoubleClickSnapModeChanged();
        });
}

void ParameterPanel::showTimelineBeatMenu()
{
    constexpr int menuBaseId = 7500;
    juce::PopupMenu menu;
    menu.setLookAndFeel(&getPitchPopupLookAndFeel());

    for (size_t i = 0; i < kTimelineBeatOptions.size(); ++i)
    {
        const auto& option = kTimelineBeatOptions[i];
        const bool selected = timelineBeatNumerator == option.numerator &&
                              timelineBeatDenominator == option.denominator;
        menu.addCustomItem(menuBaseId + static_cast<int>(i),
                           std::make_unique<HoverMenuItemComponent>(
                               option.label, selected),
                           nullptr, option.label);
    }

    auto options = juce::PopupMenu::Options()
        .withTargetComponent(&timelineBeatButton)
        .withParentComponent(this)
        .withMinimumWidth(timelineBeatButton.getWidth());

    menu.showMenuAsync(options,
                       [safeThis = juce::Component::SafePointer<ParameterPanel>(this)](int result)
                       {
                           if (safeThis == nullptr || result == 0)
                               return;

                           constexpr int menuBaseId = 7500;
                           const int idx = result - menuBaseId;
                           if (idx >= 0 && idx < static_cast<int>(kTimelineBeatOptions.size()))
                           {
                               const auto& option = kTimelineBeatOptions[static_cast<size_t>(idx)];
                               safeThis->setTimelineBeatSignatureInternal(
                                   option.numerator, option.denominator, true);
                           }
                       });
}

void ParameterPanel::showTimelineGridMenu()
{
    constexpr int menuBaseId = 7600;
    juce::PopupMenu menu;
    menu.setLookAndFeel(&getPitchPopupLookAndFeel());

    for (size_t i = 0; i < kTimelineGridOptions.size(); ++i)
    {
        const auto& option = kTimelineGridOptions[i];
        menu.addCustomItem(menuBaseId + static_cast<int>(i),
                           std::make_unique<HoverMenuItemComponent>(
                               option.label, timelineGridDivision == option.division),
                           nullptr, option.label);
    }

    auto options = juce::PopupMenu::Options()
        .withTargetComponent(&timelineGridButton)
        .withParentComponent(this)
        .withMinimumWidth(timelineGridButton.getWidth());

    menu.showMenuAsync(options,
                       [safeThis = juce::Component::SafePointer<ParameterPanel>(this)](int result)
                       {
                           if (safeThis == nullptr || result == 0)
                               return;

                           constexpr int menuBaseId = 7600;
                           const int idx = result - menuBaseId;
                           if (idx >= 0 && idx < static_cast<int>(kTimelineGridOptions.size()))
                           {
                               safeThis->setTimelineGridDivisionInternal(
                                   kTimelineGridOptions[static_cast<size_t>(idx)].division, true);
                           }
                       });
}
