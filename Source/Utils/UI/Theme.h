#pragma once

#include "../../JuceHeader.h"

// UI theme colors (defined in Theme.cpp)
extern const juce::Colour APP_COLOR_BACKGROUND;
extern const juce::Colour APP_COLOR_SURFACE;
extern const juce::Colour APP_COLOR_SURFACE_ALT;
extern const juce::Colour APP_COLOR_SURFACE_RAISED;
extern const juce::Colour APP_COLOR_BORDER;
extern const juce::Colour APP_COLOR_BORDER_SUBTLE;
extern const juce::Colour APP_COLOR_BORDER_HIGHLIGHT;
extern const juce::Colour APP_COLOR_GRID;
extern const juce::Colour APP_COLOR_GRID_BAR;
extern const juce::Colour APP_COLOR_TIMELINE;
extern const juce::Colour APP_COLOR_PIANO_WHITE;
extern const juce::Colour APP_COLOR_PIANO_BLACK;
extern const juce::Colour APP_COLOR_PIANO_TEXT;
extern const juce::Colour APP_COLOR_PIANO_TEXT_DIM;
extern const juce::Colour APP_COLOR_TEXT_PRIMARY;
extern const juce::Colour APP_COLOR_TEXT_MUTED;
extern const juce::Colour APP_COLOR_PITCH_CURVE;
extern const juce::Colour APP_COLOR_NOTE_NORMAL;
extern const juce::Colour APP_COLOR_NOTE_SELECTED;
extern const juce::Colour APP_COLOR_NOTE_HOVER;
extern const juce::Colour APP_COLOR_PRIMARY;
extern const juce::Colour APP_COLOR_PRIMARY_GLOW;
extern const juce::Colour APP_COLOR_SECONDARY;
extern const juce::Colour APP_COLOR_WAVEFORM;
extern const juce::Colour APP_COLOR_KNOB_SHADOW;
extern const juce::Colour APP_COLOR_ALERT_WARNING;
extern const juce::Colour APP_COLOR_ALERT_ERROR;
extern const juce::Colour APP_COLOR_OVERLAY_DIM;
extern const juce::Colour APP_COLOR_OVERLAY_SHADOW;
extern const juce::Colour APP_COLOR_SELECTION_OVERLAY;
extern const juce::Colour APP_COLOR_SELECTION_HIGHLIGHT;
extern const juce::Colour APP_COLOR_SELECTION_HIGHLIGHT_STRONG;
extern const juce::Colour APP_COLOR_TITLEBAR_CLOSE_MAC;
extern const juce::Colour APP_COLOR_TITLEBAR_MINIMIZE_MAC;
extern const juce::Colour APP_COLOR_TITLEBAR_MAXIMIZE_MAC;
extern const juce::Colour APP_COLOR_TITLEBAR_CLOSE_HOVER;

// ── Buttons & toggles ─────────────────────────────────────────────
extern const juce::Colour APP_COLOR_BUTTON_FILL;
extern const juce::Colour APP_COLOR_BUTTON_FILL_DOWN;
extern const juce::Colour APP_COLOR_BUTTON_TEXT;
extern const juce::Colour APP_COLOR_BUTTON_TEXT_DISABLED;
extern const juce::Colour APP_COLOR_CLOSE_BUTTON_TEXT_ON;
extern const juce::Colour APP_COLOR_TEXT_ON_PRIMARY;
extern const juce::Colour APP_COLOR_SWITCH_TRACK_ON;
extern const juce::Colour APP_COLOR_SWITCH_TRACK_OFF;
extern const juce::Colour APP_COLOR_SWITCH_THUMB_ON;
extern const juce::Colour APP_COLOR_SWITCH_THUMB_OFF;
extern const juce::Colour APP_COLOR_RADIO_SELECTED;
extern const juce::Colour APP_COLOR_RADIO_LABEL_SELECTED;
extern const juce::Colour APP_COLOR_KNOB_HIGHLIGHT;

// ── Inputs & sliders ──────────────────────────────────────────────
extern const juce::Colour APP_COLOR_CONTROL_FILL;
extern const juce::Colour APP_COLOR_CONTROL_TEXT;
extern const juce::Colour APP_COLOR_CONTROL_FOCUS_OUTLINE;
extern const juce::Colour APP_COLOR_COMBO_ARROW;
extern const juce::Colour APP_COLOR_SELECTOR_CHEVRON;
extern const juce::Colour APP_COLOR_MIDI_ICON;
extern const juce::Colour APP_COLOR_TEXT_EDIT_HIGHLIGHT;
extern const juce::Colour APP_COLOR_TEXT_EDIT_OUTLINE;
extern const juce::Colour APP_COLOR_SLIDER_TRACK;
extern const juce::Colour APP_COLOR_SLIDER_THUMB;

// ── Menus & tooltips ──────────────────────────────────────────────
extern const juce::Colour APP_COLOR_MENU_BACKGROUND;
extern const juce::Colour APP_COLOR_MENU_BORDER;
extern const juce::Colour APP_COLOR_MENU_SEPARATOR;
extern const juce::Colour APP_COLOR_MENU_ITEM_HIGHLIGHT;
extern const juce::Colour APP_COLOR_MENU_TICK;
extern const juce::Colour APP_COLOR_MENU_SHADOW;
extern const juce::Colour APP_COLOR_MENU_INNER_HIGHLIGHT;
extern const juce::Colour APP_COLOR_MENU_BAR_HIGHLIGHT;
extern const juce::Colour APP_COLOR_TOOLTIP_BACKGROUND;
extern const juce::Colour APP_COLOR_TOOLTIP_BORDER;
extern const juce::Colour APP_COLOR_TOOLTIP_TEXT;

// ── Toolbar, title bar & sidebar ──────────────────────────────────
extern const juce::Colour APP_COLOR_TOOLBAR_BACKGROUND;
extern const juce::Colour APP_COLOR_TOOLBAR_GROUP;
extern const juce::Colour APP_COLOR_TOOLBAR_PROGRESS;
extern const juce::Colour APP_COLOR_TOOL_BUTTON_SHINE;
extern const juce::Colour APP_COLOR_TITLEBAR_BUTTON_ICON;
extern const juce::Colour APP_COLOR_SIDEBAR_ICON;
extern const juce::Colour APP_COLOR_SPLASH_TITLE;

// ── Scrollbars ────────────────────────────────────────────────────
// One look for every scrollbar in the app: a dim grey rounded thumb over a
// recessed gutter. The piano-roll canvas set the look; styleCanvasScrollBar()
// is how everything else adopts it, so the side panel and the canvas cannot
// drift apart. The default JUCE renderer draws the thumb only - whoever hosts
// the bar fills APP_COLOR_SCROLLBAR_TRACK behind it.
extern const juce::Colour APP_COLOR_SCROLLBAR_THUMB;
extern const juce::Colour APP_COLOR_SCROLLBAR_TRACK;
constexpr int APP_SCROLLBAR_THICKNESS = 8;

void styleCanvasScrollBar(juce::ScrollBar& scrollBar);
