#pragma once

#include "PitchPopupMenu.h"
#include "../../Utils/Localization.h"
#include "../Buttons.h"
#include "BinaryData.h"
#include <array>

namespace canvasPopupMenu
{
class LookAndFeel final : public juce::LookAndFeel_V4
{
public:
    LookAndFeel()
    {
        setColour(juce::PopupMenu::backgroundColourId, juce::Colours::transparentBlack);
    }

    void drawPopupMenuBackground(juce::Graphics&, int, int) override {}
    void drawResizableFrame(juce::Graphics&, int, int,
                            const juce::BorderSize<int>&) override {}
    int getPopupMenuBorderSize() override { return 0; }
    // The upper action paints its own outline; keep the tool strip unframed.
    int getMenuWindowFlags() override { return 0; }
};

inline LookAndFeel& getLookAndFeel()
{
    static LookAndFeel instance;
    return instance;
}

class ActionButton final : public juce::Button
{
public:
    explicit ActionButton(const juce::String& label) : juce::Button(label) {}
    // Tool strip buttons draw a Lucide icon on a tile instead of a text row.
    ButtonIcon icon;
    // Choice rows indent their text and mark the current choice with a dot.
    bool tickable = false;
    bool ticked = false;

    void paintButton(juce::Graphics& g, bool over, bool down) override
    {
        if (icon.isValid())
        {
            // Tile steps match the former sprite: dark, lighter on hover, dark glyph when pressed.
            g.setColour(juce::Colour(over || down ? 0xFF5B5B5Bu : 0xFF2E2E2Du));
            g.fillRoundedRectangle(getLocalBounds().toFloat(), 3.0f);
            icon.states[down ? 1 : 0]->drawWithin(
                g, getLocalBounds().withSizeKeepingCentre(14, 14).toFloat(),
                juce::RectanglePlacement::centred, 1.0f);
            return;
        }
        if (isEnabled() && (over || down || hasKeyboardFocus(false)))
        {
            g.setColour(APP_COLOR_MENU_ITEM_HIGHLIGHT);
            g.fillRoundedRectangle(getLocalBounds().toFloat().reduced(2, 1), 5);
        }
        if (ticked)
        {
            g.setColour(APP_COLOR_MENU_TICK);
            g.fillEllipse(8.0f, getHeight() * 0.5f - 3.5f, 7.0f, 7.0f);
        }
        g.setColour(APP_COLOR_TEXT_PRIMARY.withMultipliedAlpha(isEnabled() ? 1.0f : 0.4f));
        g.setFont(AppFont::getFont(14));
        g.drawText(getButtonText(), getLocalBounds().withTrimmedLeft(tickable ? 22 : 12),
                   juce::Justification::centredLeft);
    }
};

class Content final : public juce::PopupMenu::CustomComponent
{
public:
    static juce::Rectangle<int> getTargetArea(juce::Point<int> mousePosition,
                                              bool withEngine = false)
    {
        const juce::Point<int> selectCentre(buttonWidth / 2,
                                           getTopHeight(withEngine) + 1 + buttonHeight / 2);
        // JUCE places the menu below the target rectangle's bottom edge.
        return { mousePosition.x - selectCentre.x,
                 mousePosition.y - selectCentre.y - 1, 1, 1 };
    }

    // engine: -2 hides the note engine rows (no note under the pointer), -1
    // shows them with none ticked (a mixed selection), 0..2 ticks a NoteEngine.
    // The rows report choices 8..10.
    Content(std::shared_ptr<int> result, bool canUndo, bool canRedo, int engine = -2)
        : juce::PopupMenu::CustomComponent(false), choice(std::move(result)),
          withEngine(engine >= -1), topHeight(getTopHeight(engine >= -1))
    {
        const juce::String names[] = {
            TR("canvas.select_all"), TR("canvas.select"), TR("canvas.split"),
            TR("canvas.pitch_drawing"),    TR("canvas.timing"), TR("command.undo"),
            TR("command.redo"), TR("engine.default"), TR("param.engine_vocoder"),
            TR("param.engine_psola")
        };
        // Same icons as the toolbar's tool buttons.
        const char* icons[] = { BinaryData::lucide_mouse_pointer_2_svg,
                                BinaryData::lucide_scissors_svg,
                                BinaryData::lucide_pencil_svg,
                                BinaryData::lucide_move_horizontal_svg };
        for (int i = 0; i < 10; ++i)
        {
            buttons[i] = std::make_unique<ActionButton>(names[i]);
            if (i > 0 && i < 5)
                buttons[i]->icon.set(icons[i - 1], nullptr,
                                     { juce::Colour(0xFF9A9A9Au), juce::Colour(0xFF2E2E2Du),
                                       {}, {} });
            buttons[i]->onClick = [this, i]
            {
                *choice = i + 1;
                triggerMenuItem();
            };
            addAndMakeVisible(*buttons[i]);
        }
        buttons[5]->setEnabled(canUndo);
        buttons[6]->setEnabled(canRedo);
        topWidth = juce::GlyphArrangement::getStringWidthInt(
                      AppFont::getFont(14), names[0]) + 24;
        engineHeader = TR("command.note_engine");
        for (int i = 7; i < 10; ++i)
        {
            buttons[i]->setVisible(withEngine);
            buttons[i]->tickable = true;
            buttons[i]->ticked = engine == i - 7;
            if (withEngine)
                topWidth = juce::jmax(topWidth, juce::GlyphArrangement::getStringWidthInt(
                                                    AppFont::getFont(14), names[i]) + 34);
        }
    }

    void getIdealSize(int& width, int& height) override
    {
        width = juce::jmax(topWidth, 4 * buttonWidth + 3);
        height = topHeight + 1 + buttonHeight;
    }

    void paint(juce::Graphics& g) override
    {
        pitchPopupMenu::getLookAndFeel().drawPopupMenuBackground(g, topWidth, topHeight);
        g.setColour(APP_COLOR_MENU_SEPARATOR);
        g.fillRect(8, padding + 2 * rowHeight + separatorHeight / 2,
                   topWidth - 16, 1);
        if (withEngine)
        {
            const int y = padding + 3 * rowHeight + separatorHeight;
            g.fillRect(8, y + separatorHeight / 2, topWidth - 16, 1);
            g.setColour(APP_COLOR_TEXT_MUTED);
            g.setFont(AppFont::getFont(13));
            g.drawText(engineHeader, 12, y + separatorHeight, topWidth - 24, rowHeight,
                       juce::Justification::centredLeft);
        }
    }

    void resized() override
    {
        buttons[5]->setBounds(0, padding, topWidth, rowHeight);
        buttons[6]->setBounds(0, padding + rowHeight, topWidth, rowHeight);
        buttons[0]->setBounds(0, padding + 2 * rowHeight + separatorHeight,
                              topWidth, rowHeight);
        for (int i = 7; i < 10; ++i)
            buttons[i]->setBounds(0, padding + (i - 3) * rowHeight + 2 * separatorHeight,
                                  topWidth, rowHeight);
        for (int i = 1; i < 5; ++i)
            buttons[i]->setBounds((i - 1) * (buttonWidth + 1), topHeight + 1,
                                  buttonWidth, buttonHeight);
    }

private:
    static constexpr int padding = 4, rowHeight = 26, separatorHeight = 8;
    static constexpr int buttonWidth = 22, buttonHeight = 21;
    // Engine block: separator, header row, three choice rows.
    static constexpr int getTopHeight(bool withEngine)
    {
        return 2 * padding + 3 * rowHeight + separatorHeight +
               (withEngine ? separatorHeight + 4 * rowHeight : 0);
    }
    int topWidth = 0;
    std::shared_ptr<int> choice;
    bool withEngine = false;
    int topHeight = 0;
    juce::String engineHeader;
    std::array<std::unique_ptr<ActionButton>, 10> buttons;
};
}
