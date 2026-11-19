/*
  ==============================================================================

    Buttons.h
    Created: 23 Jun 2025 10:21:52am
    Author:  200gaga

  ==============================================================================
*/

#pragma once
#include "../JuceHeader.h"
#include <array>

// Line icon (Lucide SVG, stroke="currentColor") pre-rendered in one colour per button state.
struct ButtonIcon
{
    void set(const char* svg, const char* svgWhenOn, std::array<juce::Colour, 4> colours);
    bool isValid() const { return states[0] != nullptr; }
    void draw(juce::Graphics& g, juce::Rectangle<int> area, int state) const;
    std::array<std::unique_ptr<juce::Drawable>, 4> states;
};

class Button : public juce::TextButton
{
public:
    Button(juce::Image img = juce::Image());
    void paintButton (juce::Graphics &g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
    void setEnabled (bool shouldBeEnabled);
    void setImage(juce::Image img);
    void setIcon(const char* svg);
    void setTextPosition(int x, int y);
private:
    juce::Image image;
    ButtonIcon icon;
    int textX, textY;
};

class ToggleButton : public juce::ToggleButton
{
public:
    ToggleButton();
    ~ToggleButton();
    void paintButton (juce::Graphics &g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
    void setEnabled (bool shouldBeEnabled);
    void setImage(juce::Image img);
    // svgWhenOn: optional second icon shown while toggled on (e.g. pause for play).
    void setIcon(const char* svg, const char* svgWhenOn = nullptr);
private:
    juce::Image image;
    ButtonIcon icon;
};
