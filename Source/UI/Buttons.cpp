/*
  ==============================================================================

    Buttons.cpp
    Created: 23 Jun 2025 10:21:52am
    Author:  200gaga

  ==============================================================================
*/

#include "Buttons.h"
#include "../Utils/UI/Theme.h"
#include "Components/AppFont.h"

namespace
{
constexpr int iconButtonSize = 24;
constexpr int iconSize = 18;
}

void ButtonIcon::set(const char* svg, const char* svgWhenOn, std::array<juce::Colour, 4> colours)
{
    for (size_t i = 0; i < states.size(); ++i)
    {
        // States 2 and 3 are the "on" states of a toggle.
        const juce::String source(i >= 2 && svgWhenOn != nullptr ? svgWhenOn : svg);
        states[i] = juce::Drawable::createFromSVGString(
            source.replace("currentColor", "#" + colours[i].toDisplayString(false)));
    }
}

void ButtonIcon::draw(juce::Graphics& g, juce::Rectangle<int> area, int state) const
{
    if (auto* d = states[(size_t) state].get())
    {
        const int size = juce::jmin(iconSize, area.getWidth(), area.getHeight());
        d->drawWithin(g, area.withSizeKeepingCentre(size, size).toFloat(),
                      juce::RectanglePlacement::centred, 1.0f);
    }
}

Button::Button(juce::Image img)
:image(img), textX(0), textY(0)
{
    if (img.isValid())
        setSize((img.getWidth() + 1) / 2, (img.getHeight() + 1) / 3 / 2);
    setMouseCursor(juce::MouseCursor::PointingHandCursor);
}

void Button::paintButton (juce::Graphics &g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    if (icon.isValid())
        icon.draw(g, getLocalBounds(), !isEnabled() ? 3 : shouldDrawButtonAsDown ? 2 : shouldDrawButtonAsHighlighted ? 1 : 0);
    else if (image.isValid())
        g.drawImage(image, (getWidth() - image.getWidth() / 2) / 2, (getHeight() - image.getHeight() / 3 / 2) / 2, image.getWidth() / 2, image.getHeight() / 3 / 2, 0, image.getHeight() * (shouldDrawButtonAsDown || !isEnabled() ? 2 : (shouldDrawButtonAsHighlighted ? 1 : 0)) / 3, image.getWidth(), image.getHeight() / 3);
    else
    {
        auto baseColour = findColour(juce::TextButton::buttonColourId).withMultipliedSaturation (hasKeyboardFocus (true) ? 1.3f : 0.9f).withMultipliedAlpha (isEnabled() ? 1.0f : 0.5f);
        if ((shouldDrawButtonAsDown || shouldDrawButtonAsHighlighted))
            baseColour = baseColour.contrasting (shouldDrawButtonAsDown ? 0.2f : 0.05f);
        g.setColour(shouldDrawButtonAsDown ? findColour(juce::TextButton::buttonOnColourId) : baseColour);
        g.fillRoundedRectangle(0, 0, getWidth(), getHeight(), getWidth() > 40 ? 13.f : 3.5f);
    }
    g.setFont(getLookAndFeel().getTextButtonFont(*this, getHeight()));
    g.setColour(isEnabled() ? findColour(shouldDrawButtonAsHighlighted ? juce::TextButton::textColourOnId : juce::TextButton::textColourOffId) : APP_COLOR_BUTTON_TEXT_DISABLED);
    g.drawText(getButtonText(), getLocalBounds().withX(textX).withY(textY), juce::Justification::centred);
}

void Button::setEnabled (bool shouldBeEnabled)
{
    juce::TextButton::setEnabled(shouldBeEnabled);
    setMouseCursor(shouldBeEnabled ? juce::MouseCursor::PointingHandCursor : juce::MouseCursor::NormalCursor);
}

void Button::setImage(juce::Image img)
{
    image = img;
    setSize((img.getWidth() + 1) / 2, (img.getHeight() + 1) / 3 / 2);
    repaint();
}

void Button::setIcon(const char* svg)
{
    icon.set(svg, nullptr, {APP_COLOR_TEXT_MUTED, APP_COLOR_TEXT_PRIMARY, APP_COLOR_PRIMARY, APP_COLOR_BUTTON_TEXT_DISABLED});
    setSize(iconButtonSize, iconButtonSize);
    repaint();
}

void Button::setTextPosition(int x, int y)
{
    textX = x;
    textY = y;
}

ToggleButton::ToggleButton()
{
    setMouseCursor(juce::MouseCursor::PointingHandCursor);
}

ToggleButton::~ToggleButton()
{
    
}

void ToggleButton::setEnabled (bool shouldBeEnabled)
{
    juce::ToggleButton::setEnabled(shouldBeEnabled);
    setMouseCursor(shouldBeEnabled ? juce::MouseCursor::PointingHandCursor : juce::MouseCursor::NormalCursor);
}

void ToggleButton::setImage(juce::Image img)
{
    image = img;
    setSize((img.getWidth() + 1) / 2, (img.getHeight() + 1) / 4 / 2);
    repaint();
}

void ToggleButton::paintButton (juce::Graphics &g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    setAlpha(isEnabled() ? 1.f : 0.3f);
    if (icon.isValid())
        icon.draw(g, getLocalBounds(), (shouldDrawButtonAsHighlighted ? 1 : 0) + (getToggleState() ? 2 : 0));
    else if (image.isValid())
    {
        g.drawImage(image, (getWidth() - (image.getWidth() + 1) / 2) / 2, (getHeight() - (image.getHeight() / 4 + 1) / 2) / 2, (image.getWidth() + 1) / 2, (image.getHeight() / 4  + 1) / 2, 0, ((shouldDrawButtonAsHighlighted ? 1 : 0) + (getToggleState() ? 2 : 0)) * image.getHeight() / 4 , image.getWidth(), image.getHeight() / 4);
    }
    else
    {
        g.setColour(shouldDrawButtonAsHighlighted ? APP_COLOR_TOGGLE_FILL_HOVER : APP_COLOR_TOGGLE_FILL);
        g.fillRoundedRectangle(0, 0, getWidth(), getHeight(), getWidth() > 40 ? 13.f : 3.5f);
    }
    g.setFont(AppFont::getFont(14.f).withPointHeight(14.f));
    g.setColour(getToggleState() ? APP_COLOR_TOGGLE_TEXT_ON : APP_COLOR_TOGGLE_TEXT_OFF);
    g.drawText(getButtonText(), getLocalBounds(), juce::Justification::centred);
}
void ToggleButton::setIcon(const char* svg, const char* svgWhenOn)
{
    icon.set(svg, svgWhenOn, {APP_COLOR_TEXT_MUTED, APP_COLOR_TEXT_PRIMARY, APP_COLOR_PRIMARY, APP_COLOR_PRIMARY_GLOW});
    setSize(iconButtonSize, iconButtonSize);
    repaint();
}
