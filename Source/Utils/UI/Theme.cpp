#include "Theme.h"

// Premium dark theme palette — refined depth with warm accent highlights
// Background hierarchy: BACKGROUND < SURFACE < SURFACE_ALT < SURFACE_RAISED
// Each layer is subtly brighter, creating natural elevation perception.

// ── Core backgrounds ──────────────────────────────────────────────
const juce::Colour APP_COLOR_BACKGROUND          = juce::Colour(0xFF1C1C1Cu);  // Deep charcoal base
const juce::Colour APP_COLOR_SURFACE             = juce::Colour(0xFF161922u);  // Primary surface
const juce::Colour APP_COLOR_SURFACE_ALT         = juce::Colour(0xFF0D0B0Bu);  // Recessed/inset surface (controls, inputs)
const juce::Colour APP_COLOR_SURFACE_RAISED      = juce::Colour(0xFF222737u);  // Elevated cards/popups

// ── Borders ───────────────────────────────────────────────────────
const juce::Colour APP_COLOR_BORDER              = juce::Colour(0xFF282E40u);  // Standard border
const juce::Colour APP_COLOR_BORDER_SUBTLE       = juce::Colour(0xFF1E2333u);  // Barely visible divider
const juce::Colour APP_COLOR_BORDER_HIGHLIGHT    = juce::Colour(0xFF3D4562u);  // Focus / active border

// ── Piano roll grid & timeline ────────────────────────────────────
const juce::Colour APP_COLOR_GRID                = juce::Colour(0xFF0D0B0Bu);  // Subdivision lines
const juce::Colour APP_COLOR_GRID_BAR            = juce::Colour(0xFF0D0B0Bu);  // Bar / beat lines
const juce::Colour APP_COLOR_TIMELINE            = juce::Colour(0xFF131620u);  // Timeline ruler background

// ── Piano keys ────────────────────────────────────────────────────
const juce::Colour APP_COLOR_PIANO_WHITE         = juce::Colour(0xFF272D40u);  // White key face
const juce::Colour APP_COLOR_PIANO_BLACK         = juce::Colour(0xFF171B28u);  // Black key face
const juce::Colour APP_COLOR_PIANO_TEXT          = juce::Colour(0xFFD0D6E8u);  // Active note label
const juce::Colour APP_COLOR_PIANO_TEXT_DIM      = juce::Colour(0xFF7E8899u);  // Inactive note label

// ── Text ──────────────────────────────────────────────────────────
const juce::Colour APP_COLOR_TEXT_PRIMARY         = juce::Colour(0xFFECEFF5u);  // High-contrast primary text
const juce::Colour APP_COLOR_TEXT_MUTED           = juce::Colour(0xFF8891A6u);  // Secondary / caption text

// ── Pitch & notes ─────────────────────────────────────────────────
const juce::Colour APP_COLOR_PITCH_CURVE          = juce::Colour(0xFFE6E6E6u);  // Pitch contour line
const juce::Colour APP_COLOR_NOTE_NORMAL          = juce::Colour(0xFF7C6AFFu);  // Default note block
const juce::Colour APP_COLOR_NOTE_SELECTED        = juce::Colour(0xFFA594FFu);  // Selected note (brighter)
const juce::Colour APP_COLOR_NOTE_HOVER           = juce::Colour(0xFF8E7EFFu);  // Hovered note

// ── Accent colours ────────────────────────────────────────────────
const juce::Colour APP_COLOR_PRIMARY              = juce::Colour(0xFF7C6AFFu);  // Brand purple
const juce::Colour APP_COLOR_PRIMARY_GLOW         = juce::Colour(0xFFBAAEFFu);  // Glow / highlight ring
const juce::Colour APP_COLOR_SECONDARY            = juce::Colour(0xFF44D1DBu);  // Teal-cyan complement

// ── Waveform ──────────────────────────────────────────────────────
const juce::Colour APP_COLOR_WAVEFORM             = juce::Colour(0xFF232940u);  // Waveform background tint

// ── Knobs & shadows ───────────────────────────────────────────────
const juce::Colour APP_COLOR_KNOB_SHADOW          = juce::Colour(0xFF08090Du);  // Deep knob drop-shadow

// ── Alerts ────────────────────────────────────────────────────────
const juce::Colour APP_COLOR_ALERT_WARNING        = juce::Colour(0xFFFFB626u);  // Warm amber warning
const juce::Colour APP_COLOR_ALERT_ERROR          = juce::Colour(0xFFFF4D5Au);  // Softer red error

// ── Overlays & selections ─────────────────────────────────────────
const juce::Colour APP_COLOR_OVERLAY_DIM          = juce::Colour(0xC0000008u);  // Modal backdrop (slightly blue-black)
const juce::Colour APP_COLOR_OVERLAY_SHADOW       = juce::Colour(0xA0000008u);  // Drop-shadow fill
const juce::Colour APP_COLOR_SELECTION_OVERLAY    = juce::Colour(0x18000000u);  // Rubber-band fill
const juce::Colour APP_COLOR_SELECTION_HIGHLIGHT  = juce::Colour(0x406C5AFFu);  // Selection tint (matches primary)
const juce::Colour APP_COLOR_SELECTION_HIGHLIGHT_STRONG = juce::Colour(0xC06C5AFFu); // Strong selection

// ── Title bar (macOS traffic lights & Windows close hover) ────────
const juce::Colour APP_COLOR_TITLEBAR_CLOSE_MAC    = juce::Colour(0xFFFF5F57u);
const juce::Colour APP_COLOR_TITLEBAR_MINIMIZE_MAC = juce::Colour(0xFFFEBC2Eu);
const juce::Colour APP_COLOR_TITLEBAR_MAXIMIZE_MAC = juce::Colour(0xFF28C840u);
const juce::Colour APP_COLOR_TITLEBAR_CLOSE_HOVER  = juce::Colour(0xFFE81123u);

// ── Buttons & toggles ─────────────────────────────────────────────
const juce::Colour APP_COLOR_BUTTON_FILL           = juce::Colour(0xFF5B5B5Bu);  // Text button fill
const juce::Colour APP_COLOR_BUTTON_FILL_DOWN      = juce::Colour(0xFF3E3E3Eu);  // Text button fill while pressed / on
const juce::Colour APP_COLOR_BUTTON_TEXT           = juce::Colour(0xFFEFEFEFu);  // Text button label
const juce::Colour APP_COLOR_BUTTON_TEXT_DISABLED  = juce::Colour(0xFF3E3E3Eu);  // Disabled text button label
const juce::Colour APP_COLOR_CLOSE_BUTTON_TEXT_ON  = juce::Colour(0xFFFFFFFFu);  // Settings close button label while on
const juce::Colour APP_COLOR_TEXT_ON_PRIMARY       = juce::Colour(0xFFFFFFFFu);  // Label on a primary-coloured button
const juce::Colour APP_COLOR_SWITCH_TRACK_ON       = juce::Colour(0xFF575757u);  // Switch track, on
const juce::Colour APP_COLOR_SWITCH_TRACK_OFF      = juce::Colour(0xFF30302Eu);  // Switch track, off
const juce::Colour APP_COLOR_SWITCH_THUMB_ON       = juce::Colour(0xFFFFFFFFu);  // Switch thumb, on
const juce::Colour APP_COLOR_SWITCH_THUMB_OFF      = juce::Colour(0xFF575757u);  // Switch thumb, off
const juce::Colour APP_COLOR_RADIO_SELECTED        = juce::Colour(0xFFFFFFFFu);  // Radio button ring and dot, selected
const juce::Colour APP_COLOR_RADIO_LABEL_SELECTED  = juce::Colour(0xFFE6E6E6u);  // Radio button label, selected
const juce::Colour APP_COLOR_KNOB_HIGHLIGHT        = juce::Colour(0x0AFFFFFFu);  // Knob top highlight crescent

// ── Inputs & sliders ──────────────────────────────────────────────
const juce::Colour APP_COLOR_CONTROL_FILL          = juce::Colour(0xFF30302Eu);  // Combo boxes, selector buttons, value boxes
const juce::Colour APP_COLOR_CONTROL_TEXT          = juce::Colour(0xFFE6E6E6u);  // Text on CONTROL_FILL
const juce::Colour APP_COLOR_CONTROL_FOCUS_OUTLINE = juce::Colour(0xFF3E3E3Eu);  // Focused combo box outline
const juce::Colour APP_COLOR_COMBO_ARROW           = juce::Colour(0xFFE6E6E6u);  // Combo box arrow
const juce::Colour APP_COLOR_SELECTOR_CHEVRON      = juce::Colour(0xFF9B9B9Bu);  // Chevron on combo-style selector buttons
const juce::Colour APP_COLOR_MIDI_ICON             = juce::Colour(0xFF9B9B9Bu);  // MIDI file / drag button icon
const juce::Colour APP_COLOR_TEXT_EDIT_HIGHLIGHT   = juce::Colour(0xFF7A7D8Bu);  // Selected text in inline value editors
const juce::Colour APP_COLOR_TEXT_EDIT_OUTLINE     = juce::Colour(0xFF3C3C3Cu);  // Inline value editor outline
const juce::Colour APP_COLOR_SLIDER_TRACK          = juce::Colour(0xFF494949u);  // Linear slider track
const juce::Colour APP_COLOR_SLIDER_THUMB          = juce::Colour(0xFF9B9B9Bu);  // Linear slider thumb

// ── Menus & tooltips ──────────────────────────────────────────────
const juce::Colour APP_COLOR_MENU_BACKGROUND       = juce::Colour(0xFF30302Eu);  // Popup menu fill
const juce::Colour APP_COLOR_MENU_BORDER           = juce::Colour(0xFF3E3E3Eu);  // Popup menu outline
const juce::Colour APP_COLOR_MENU_SEPARATOR        = juce::Colour(0xFF3E3E3Eu);  // Popup menu separator line
const juce::Colour APP_COLOR_MENU_ITEM_HIGHLIGHT   = juce::Colour(0xFF171717u);  // Hovered popup menu item
const juce::Colour APP_COLOR_MENU_TICK             = juce::Colour(0xFFEFEFEFu);  // Ticked popup menu item dot
const juce::Colour APP_COLOR_MENU_SHADOW           = juce::Colour(0x30000000u);  // Default popup menu outer shadow
const juce::Colour APP_COLOR_MENU_INNER_HIGHLIGHT  = juce::Colour(0x0CFFFFFFu);  // Default popup menu top-edge highlight
const juce::Colour APP_COLOR_MENU_BAR_HIGHLIGHT    = juce::Colour(0xFF191818u);  // Hovered / open menu bar item
const juce::Colour APP_COLOR_TOOLTIP_BACKGROUND    = juce::Colour(0xFF232323u);  // Tooltip fill
const juce::Colour APP_COLOR_TOOLTIP_BORDER        = juce::Colour(0xFF3C3C3Cu);  // Tooltip outline
const juce::Colour APP_COLOR_TOOLTIP_TEXT          = juce::Colour(0xFF9B9B9Bu);  // Tooltip text

// ── Toolbar, title bar & sidebar ──────────────────────────────────
const juce::Colour APP_COLOR_TOOLBAR_BACKGROUND    = juce::Colour(0xFF0D0B0Bu);  // Toolbar fill
const juce::Colour APP_COLOR_TOOLBAR_GROUP         = juce::Colour(0xFF191818u);  // Capsules grouping toolbar buttons
const juce::Colour APP_COLOR_TOOLBAR_PROGRESS      = juce::Colour(0xFFFF5600u);  // Toolbar progress bar fill
const juce::Colour APP_COLOR_TOOL_BUTTON_SHINE     = juce::Colour(0x18FFFFFFu);  // Inner top highlight on the active tool button
const juce::Colour APP_COLOR_TITLEBAR_BUTTON_ICON  = juce::Colour(0xFF000000u);  // Glyph drawn on hovered title bar buttons
const juce::Colour APP_COLOR_SIDEBAR_ICON          = juce::Colour(0xFFFFFFFFu);  // Sidebar SVG icon tint
const juce::Colour APP_COLOR_SPLASH_TITLE          = juce::Colour(0xFFFFFFFFu);  // Splash screen title

// ── Panels & dialogs ──────────────────────────────────────────────
const juce::Colour APP_COLOR_PANEL_BACKGROUND            = juce::Colour(0xFF232323u);  // Side panel fill
const juce::Colour APP_COLOR_PANEL_BORDER                = juce::Colour(0xFF3C3C3Cu);  // Side panel outline
const juce::Colour APP_COLOR_PANEL_CARD                  = juce::Colour(0xFF171717u);  // Cards inside the parameter panel
const juce::Colour APP_COLOR_PANEL_SECTION_TITLE         = juce::Colour(0xFF9B9B9Bu);  // Parameter panel section titles
const juce::Colour APP_COLOR_PANEL_LABEL                 = juce::Colour(0xFFE6E6E6u);  // Parameter panel row labels
const juce::Colour APP_COLOR_DIALOG_BACKGROUND           = juce::Colour(0xFF232323u);  // Settings and export dialog fill
const juce::Colour APP_COLOR_DIALOG_CARD                 = juce::Colour(0xFF151515u);  // Cards inside settings and export dialogs
const juce::Colour APP_COLOR_POPOVER_BACKGROUND          = juce::Colour(0xFF181818u);  // Small popover dialogs (quantize pitch)
const juce::Colour APP_COLOR_MODAL_BACKDROP              = juce::Colour(0xFF000000u);  // Dimmed backdrop behind modal popups
const juce::Colour APP_COLOR_MODAL_BACKGROUND            = juce::Colour(0xFF333333u);  // Modal popup fill (update, about, analysis)
const juce::Colour APP_COLOR_MODAL_TEXT                  = juce::Colour(0xFFEFEFEFu);  // Modal popup text
const juce::Colour APP_COLOR_MODAL_TEXT_AREA             = juce::Colour(0xFF191717u);  // Read-only text areas inside modal popups
const juce::Colour APP_COLOR_PROGRESS_TRACK              = juce::Colour(0xFF191717u);  // Analysis progress bar track
const juce::Colour APP_COLOR_PROGRESS_FILL               = juce::Colour(0xFF9A9A9Au);  // Analysis progress bar fill
const juce::Colour APP_COLOR_TOOLBAR_DIVIDER             = juce::Colour(0xFF3C3C3Cu);  // Line under the toolbar above the main view
const juce::Colour APP_COLOR_FLOATING_CONTROL_BACKGROUND = juce::Colour(0xFF0D0B0Bu);  // Controls floating over the piano roll
const juce::Colour APP_COLOR_OVERVIEW_BACKGROUND         = juce::Colour(0xFF191717u);  // Overview strip fill
const juce::Colour APP_COLOR_OVERVIEW_BORDER             = juce::Colour(0xFF0D0B0Bu);  // Overview strip outline
const juce::Colour APP_COLOR_CANVAS_PROMPT_BACKGROUND    = juce::Colour(0xFF2E2E2Du);  // Prompt panels over the canvas (pitch drawing)

// ── Scrollbars ────────────────────────────────────────────────────
const juce::Colour APP_COLOR_SCROLLBAR_THUMB      = juce::Colour(0xFF565656u);  // Thumb
const juce::Colour APP_COLOR_SCROLLBAR_TRACK      = juce::Colour(0xFF0D0B0Bu);  // Recessed gutter

void styleCanvasScrollBar(juce::ScrollBar& scrollBar)
{
    // Colours only - the default look and feel does the drawing, which is what
    // keeps this identical to the piano-roll bars. That renderer paints the
    // thumb and nothing else, so the gutter behind the bar is the host
    // component's job (see PianoRollComponent::paint, DraggablePanel::paint).
    scrollBar.setColour(juce::ScrollBar::thumbColourId, APP_COLOR_SCROLLBAR_THUMB);
    scrollBar.setColour(juce::ScrollBar::trackColourId, APP_COLOR_SCROLLBAR_TRACK);
    scrollBar.setColour(juce::ScrollBar::backgroundColourId, APP_COLOR_SCROLLBAR_TRACK);
}
