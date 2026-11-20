#include "Theme.h"

// Sonaria palette "Ink / Lilac": near-black ink greys, one lilac accent, orange for off-pitch notes
// Background hierarchy: BACKGROUND < SURFACE < SURFACE_ALT < SURFACE_RAISED
// Each layer is subtly brighter, creating natural elevation perception.

// ── Core backgrounds ──────────────────────────────────────────────
const juce::Colour APP_COLOR_BACKGROUND          = juce::Colour(0xFF0F0F11u);  // Deep charcoal base
const juce::Colour APP_COLOR_SURFACE             = juce::Colour(0xFF16161Au);  // Primary surface
const juce::Colour APP_COLOR_SURFACE_ALT         = juce::Colour(0xFF0B0B0Du);  // Recessed/inset surface (controls, inputs)
const juce::Colour APP_COLOR_SURFACE_RAISED      = juce::Colour(0xFF1F1F25u);  // Elevated cards/popups

// ── Borders ───────────────────────────────────────────────────────
const juce::Colour APP_COLOR_BORDER              = juce::Colour(0xFF2B2B33u);  // Standard border
const juce::Colour APP_COLOR_BORDER_SUBTLE       = juce::Colour(0xFF1F1F25u);  // Barely visible divider
const juce::Colour APP_COLOR_BORDER_HIGHLIGHT    = juce::Colour(0xFF3A3A45u);  // Focus / active border

// ── Piano roll grid & timeline ────────────────────────────────────
const juce::Colour APP_COLOR_GRID                = juce::Colour(0xFF0C0C0Du);  // Subdivision lines
const juce::Colour APP_COLOR_GRID_BAR            = juce::Colour(0xFF0C0C0Du);  // Bar / beat lines
const juce::Colour APP_COLOR_TIMELINE            = juce::Colour(0xFF131316u);  // Timeline ruler background

// ── Piano keys ────────────────────────────────────────────────────
const juce::Colour APP_COLOR_PIANO_WHITE         = juce::Colour(0xFF1F1F25u);  // White key face
const juce::Colour APP_COLOR_PIANO_BLACK         = juce::Colour(0xFF131316u);  // Black key face
const juce::Colour APP_COLOR_PIANO_TEXT          = juce::Colour(0xFFD6D3DEu);  // Active note label
const juce::Colour APP_COLOR_PIANO_TEXT_DIM      = juce::Colour(0xFF7D788Au);  // Inactive note label

// ── Text ──────────────────────────────────────────────────────────
const juce::Colour APP_COLOR_TEXT_PRIMARY         = juce::Colour(0xFFECEAF2u);  // High-contrast primary text
const juce::Colour APP_COLOR_TEXT_MUTED           = juce::Colour(0xFF9C97A8u);  // Secondary / caption text

// ── Pitch & notes ─────────────────────────────────────────────────
const juce::Colour APP_COLOR_PITCH_CURVE          = juce::Colour(0xFFE6E3F0u);  // Pitch contour line
const juce::Colour APP_COLOR_NOTE_NORMAL          = juce::Colour(0xFFB3A4FFu);  // Default note block
const juce::Colour APP_COLOR_NOTE_SELECTED        = juce::Colour(0xFFCCC1FFu);  // Selected note (brighter)
const juce::Colour APP_COLOR_NOTE_HOVER           = juce::Colour(0xFFC0B3FFu);  // Hovered note

// ── Accent colours ────────────────────────────────────────────────
const juce::Colour APP_COLOR_PRIMARY              = juce::Colour(0xFFB3A4FFu);  // Brand purple
const juce::Colour APP_COLOR_PRIMARY_GLOW         = juce::Colour(0xFFD9D1FFu);  // Glow / highlight ring
const juce::Colour APP_COLOR_SECONDARY            = juce::Colour(0xFF44D1DBu);  // Teal-cyan complement

// ── Waveform ──────────────────────────────────────────────────────
const juce::Colour APP_COLOR_WAVEFORM             = juce::Colour(0xFF26262Eu);  // Waveform background tint

// ── Knobs & shadows ───────────────────────────────────────────────
const juce::Colour APP_COLOR_KNOB_SHADOW          = juce::Colour(0xFF08080Au);  // Deep knob drop-shadow

// ── Alerts ────────────────────────────────────────────────────────
const juce::Colour APP_COLOR_ALERT_WARNING        = juce::Colour(0xFFFFB626u);  // Warm amber warning
const juce::Colour APP_COLOR_ALERT_ERROR          = juce::Colour(0xFFFF4D5Au);  // Softer red error

// ── Overlays & selections ─────────────────────────────────────────
const juce::Colour APP_COLOR_OVERLAY_DIM          = juce::Colour(0xC0000008u);  // Modal backdrop (slightly blue-black)
const juce::Colour APP_COLOR_OVERLAY_SHADOW       = juce::Colour(0xA0000008u);  // Drop-shadow fill
const juce::Colour APP_COLOR_SELECTION_OVERLAY    = juce::Colour(0x18000000u);  // Rubber-band fill
const juce::Colour APP_COLOR_SELECTION_HIGHLIGHT  = juce::Colour(0x40B3A4FFu);  // Selection tint (matches primary)
const juce::Colour APP_COLOR_SELECTION_HIGHLIGHT_STRONG = juce::Colour(0xC0B3A4FFu); // Strong selection

// ── Title bar (macOS traffic lights & Windows close hover) ────────
const juce::Colour APP_COLOR_TITLEBAR_CLOSE_MAC    = juce::Colour(0xFFFF5F57u);
const juce::Colour APP_COLOR_TITLEBAR_MINIMIZE_MAC = juce::Colour(0xFFFEBC2Eu);
const juce::Colour APP_COLOR_TITLEBAR_MAXIMIZE_MAC = juce::Colour(0xFF28C840u);
const juce::Colour APP_COLOR_TITLEBAR_CLOSE_HOVER  = juce::Colour(0xFFE81123u);

// ── Buttons & toggles ─────────────────────────────────────────────
const juce::Colour APP_COLOR_BUTTON_FILL           = juce::Colour(0xFF5B5A65u);  // Text button fill
const juce::Colour APP_COLOR_BUTTON_FILL_DOWN      = juce::Colour(0xFF3E3D45u);  // Text button fill while pressed / on
const juce::Colour APP_COLOR_BUTTON_TEXT           = juce::Colour(0xFFEFEBF9u);  // Text button label
const juce::Colour APP_COLOR_BUTTON_TEXT_DISABLED  = juce::Colour(0xFF3E3D45u);  // Disabled text button label
const juce::Colour APP_COLOR_CLOSE_BUTTON_TEXT_ON  = juce::Colour(0xFFFFFFFFu);  // Settings close button label while on
const juce::Colour APP_COLOR_TEXT_ON_PRIMARY       = juce::Colour(0xFF17122Cu);  // Label on a primary-coloured button
const juce::Colour APP_COLOR_SWITCH_TRACK_ON       = juce::Colour(0xFFB3A4FFu);  // Switch track, on
const juce::Colour APP_COLOR_SWITCH_TRACK_OFF      = juce::Colour(0xFF2F2E35u);  // Switch track, off
const juce::Colour APP_COLOR_SWITCH_THUMB_ON       = juce::Colour(0xFF17122Cu);  // Switch thumb, on
const juce::Colour APP_COLOR_SWITCH_THUMB_OFF      = juce::Colour(0xFF575661u);  // Switch thumb, off
const juce::Colour APP_COLOR_RADIO_SELECTED        = juce::Colour(0xFFB3A4FFu);  // Radio button ring and dot, selected
const juce::Colour APP_COLOR_RADIO_LABEL_SELECTED  = juce::Colour(0xFFE6E3F0u);  // Radio button label, selected
const juce::Colour APP_COLOR_KNOB_HIGHLIGHT        = juce::Colour(0x0AFFFFFFu);  // Knob top highlight crescent

// ── Inputs & sliders ──────────────────────────────────────────────
const juce::Colour APP_COLOR_CONTROL_FILL          = juce::Colour(0xFF2F2E35u);  // Combo boxes, selector buttons, value boxes
const juce::Colour APP_COLOR_CONTROL_TEXT          = juce::Colour(0xFFE6E3F0u);  // Text on CONTROL_FILL
const juce::Colour APP_COLOR_CONTROL_FOCUS_OUTLINE = juce::Colour(0xFF3E3D45u);  // Focused combo box outline
const juce::Colour APP_COLOR_COMBO_ARROW           = juce::Colour(0xFFE6E3F0u);  // Combo box arrow
const juce::Colour APP_COLOR_SELECTOR_CHEVRON      = juce::Colour(0xFF9B99A5u);  // Chevron on combo-style selector buttons
const juce::Colour APP_COLOR_MIDI_ICON             = juce::Colour(0xFF9B99A5u);  // MIDI file / drag button icon
const juce::Colour APP_COLOR_TEXT_EDIT_HIGHLIGHT   = juce::Colour(0xFF4E4580u);  // Selected text in inline value editors
const juce::Colour APP_COLOR_TEXT_EDIT_OUTLINE     = juce::Colour(0xFF3C3B43u);  // Inline value editor outline
const juce::Colour APP_COLOR_SLIDER_TRACK          = juce::Colour(0xFF494852u);  // Linear slider track
const juce::Colour APP_COLOR_SLIDER_THUMB          = juce::Colour(0xFF9B99A5u);  // Linear slider thumb

// ── Menus & tooltips ──────────────────────────────────────────────
const juce::Colour APP_COLOR_MENU_BACKGROUND       = juce::Colour(0xFF2F2E35u);  // Popup menu fill
const juce::Colour APP_COLOR_MENU_BORDER           = juce::Colour(0xFF3E3D45u);  // Popup menu outline
const juce::Colour APP_COLOR_MENU_SEPARATOR        = juce::Colour(0xFF3E3D45u);  // Popup menu separator line
const juce::Colour APP_COLOR_MENU_ITEM_HIGHLIGHT   = juce::Colour(0xFF17171Au);  // Hovered popup menu item
const juce::Colour APP_COLOR_MENU_TICK             = juce::Colour(0xFFB3A4FFu);  // Ticked popup menu item dot
const juce::Colour APP_COLOR_MENU_SHADOW           = juce::Colour(0x30000000u);  // Default popup menu outer shadow
const juce::Colour APP_COLOR_MENU_INNER_HIGHLIGHT  = juce::Colour(0x0CFFFFFFu);  // Default popup menu top-edge highlight
const juce::Colour APP_COLOR_MENU_BAR_HIGHLIGHT    = juce::Colour(0xFF18181Bu);  // Hovered / open menu bar item
const juce::Colour APP_COLOR_TOOLTIP_BACKGROUND    = juce::Colour(0xFF232227u);  // Tooltip fill
const juce::Colour APP_COLOR_TOOLTIP_BORDER        = juce::Colour(0xFF3C3B43u);  // Tooltip outline
const juce::Colour APP_COLOR_TOOLTIP_TEXT          = juce::Colour(0xFF9B99A5u);  // Tooltip text

// ── Toolbar, title bar & sidebar ──────────────────────────────────
const juce::Colour APP_COLOR_TOOLBAR_BACKGROUND    = juce::Colour(0xFF0C0C0Du);  // Toolbar fill
const juce::Colour APP_COLOR_TOOLBAR_GROUP         = juce::Colour(0xFF18181Bu);  // Capsules grouping toolbar buttons
const juce::Colour APP_COLOR_TOOLBAR_PROGRESS      = juce::Colour(0xFFB3A4FFu);  // Toolbar progress bar fill
const juce::Colour APP_COLOR_TOOL_BUTTON_SHINE     = juce::Colour(0x18FFFFFFu);  // Inner top highlight on the active tool button
const juce::Colour APP_COLOR_TITLEBAR_BUTTON_ICON  = juce::Colour(0xFF000000u);  // Glyph drawn on hovered title bar buttons
const juce::Colour APP_COLOR_SIDEBAR_ICON          = juce::Colour(0xFFFFFFFFu);  // Sidebar SVG icon tint
const juce::Colour APP_COLOR_SPLASH_TITLE          = juce::Colour(0xFFFFFFFFu);  // Splash screen title

// ── Panels & dialogs ──────────────────────────────────────────────
const juce::Colour APP_COLOR_PANEL_BACKGROUND            = juce::Colour(0xFF232227u);  // Side panel fill
const juce::Colour APP_COLOR_PANEL_BORDER                = juce::Colour(0xFF3C3B43u);  // Side panel outline
const juce::Colour APP_COLOR_PANEL_CARD                  = juce::Colour(0xFF17171Au);  // Cards inside the parameter panel
const juce::Colour APP_COLOR_PANEL_SECTION_TITLE         = juce::Colour(0xFF9B99A5u);  // Parameter panel section titles
const juce::Colour APP_COLOR_PANEL_LABEL                 = juce::Colour(0xFFE6E3F0u);  // Parameter panel row labels
const juce::Colour APP_COLOR_DIALOG_BACKGROUND           = juce::Colour(0xFF232227u);  // Settings and export dialog fill
const juce::Colour APP_COLOR_DIALOG_CARD                 = juce::Colour(0xFF151518u);  // Cards inside settings and export dialogs
const juce::Colour APP_COLOR_POPOVER_BACKGROUND          = juce::Colour(0xFF18181Bu);  // Small popover dialogs (quantize pitch)
const juce::Colour APP_COLOR_MODAL_BACKDROP              = juce::Colour(0xFF000000u);  // Dimmed backdrop behind modal popups
const juce::Colour APP_COLOR_MODAL_BACKGROUND            = juce::Colour(0xFF333239u);  // Modal popup fill (update, about, analysis)
const juce::Colour APP_COLOR_MODAL_TEXT                  = juce::Colour(0xFFEFEBF9u);  // Modal popup text
const juce::Colour APP_COLOR_MODAL_TEXT_AREA             = juce::Colour(0xFF18181Bu);  // Read-only text areas inside modal popups
const juce::Colour APP_COLOR_PROGRESS_TRACK              = juce::Colour(0xFF18181Bu);  // Analysis progress bar track
const juce::Colour APP_COLOR_PROGRESS_FILL               = juce::Colour(0xFF9A98A4u);  // Analysis progress bar fill
const juce::Colour APP_COLOR_TOOLBAR_DIVIDER             = juce::Colour(0xFF3C3B43u);  // Line under the toolbar above the main view
const juce::Colour APP_COLOR_FLOATING_CONTROL_BACKGROUND = juce::Colour(0xFF0C0C0Du);  // Controls floating over the piano roll
const juce::Colour APP_COLOR_OVERVIEW_BACKGROUND         = juce::Colour(0xFF18181Bu);  // Overview strip fill
const juce::Colour APP_COLOR_OVERVIEW_BORDER             = juce::Colour(0xFF0C0C0Du);  // Overview strip outline
const juce::Colour APP_COLOR_CANVAS_PROMPT_BACKGROUND    = juce::Colour(0xFF2E2D34u);  // Prompt panels over the canvas (pitch drawing)

// ── Piano roll canvas ─────────────────────────────────────────────
const juce::Colour APP_COLOR_RULER_BACKGROUND         = juce::Colour(0xFF232227u);  // Timeline / loop ruler and canvas header fill
const juce::Colour APP_COLOR_RULER_LINE               = juce::Colour(0xFF0C0C0Du);  // Ruler markers and bottom edge
const juce::Colour APP_COLOR_RULER_TEXT               = juce::Colour(0xFF9B99A5u);  // Ruler labels
const juce::Colour APP_COLOR_LOOP_MARKER              = juce::Colour(0xFF888692u);  // Loop range edges, loop on
const juce::Colour APP_COLOR_LOOP_MARKER_DISABLED     = juce::Colour(0xFF3E3D45u);  // Loop range edges, loop off
const juce::Colour APP_COLOR_LOOP_OVERLAY             = juce::Colour(0xFFFFFFFFu);  // Loop range tint on the canvas
const juce::Colour APP_COLOR_PIANO_KEY                = juce::Colour(0xFF232227u);  // Piano key, no scale overlay
const juce::Colour APP_COLOR_PIANO_KEY_IN_SCALE       = juce::Colour(0xFF1C1C1Fu);  // Piano key inside the active scale
const juce::Colour APP_COLOR_PIANO_KEY_OUT_OF_SCALE   = juce::Colour(0xFF151518u);  // Piano key outside the active scale
const juce::Colour APP_COLOR_PIANO_KEY_SEPARATOR      = juce::Colour(0xFF0C0C0Du);  // Line between piano keys
const juce::Colour APP_COLOR_PIANO_KEY_TEXT           = juce::Colour(0xFF9A98A4u);  // Piano key note names
const juce::Colour APP_COLOR_PIANO_KEYS_EDGE          = juce::Colour(0xFF0C0C0Du);  // Line between piano keys and the grid
const juce::Colour APP_COLOR_GRID_ROW_WHITE_KEY       = juce::Colour(0xFF1C1C1Fu);  // Grid row for a white key / in-scale note
const juce::Colour APP_COLOR_GRID_ROW_BLACK_KEY       = juce::Colour(0xFF151518u);  // Grid row for a black key / out-of-scale note
const juce::Colour APP_COLOR_CANVAS_BORDER            = juce::Colour(0xFF3C3B43u);  // Outline around the piano roll canvas
const juce::Colour APP_COLOR_SELECTION_BOX            = juce::Colour(0xFFFFFFFFu);  // Rubber-band selection fill and outline
const juce::Colour APP_COLOR_REGION_OVERLAY           = juce::Colour(0xFFFFFFFFu);  // Clip / region range tint and edges
const juce::Colour APP_COLOR_WAVEFORM_FILL            = juce::Colour(0xFFFFFFFFu);  // Background waveform behind notes
const juce::Colour APP_COLOR_MIDI_REFERENCE           = juce::Colour(0xFF73D3E8u);  // Reference MIDI notes
const juce::Colour APP_COLOR_TIMING_BOUNDARY_HOVER    = juce::Colour(0xFFFFFFFFu);  // Timing boundary, hovered or dragged
const juce::Colour APP_COLOR_TIMING_BOUNDARY_SELECTED = juce::Colour(0xFFFFFFFFu);  // Timing boundary, selected

// ── Notes & labels ────────────────────────────────────────────────
const juce::Colour APP_COLOR_NOTE_GREYSCALE           = juce::Colour(0xFFCCC9D6u);  // Note body in greyscale mode
const juce::Colour APP_COLOR_NOTE_GREYSCALE_EDGE      = juce::Colour(0xFF3A3941u);  // Note edge in greyscale mode
const juce::Colour APP_COLOR_NOTE_HOVER_SHADOW        = juce::Colour(0xFFFFFFFFu);  // Hover highlight behind a note or breath
const juce::Colour APP_COLOR_NOTE_PLAYBACK_PREVIEW    = juce::Colour(0xFFFFFFFFu);  // Note body while preview playback runs
const juce::Colour APP_COLOR_NOTE_SELECTION_OUTLINE   = juce::Colour(0xFFFFFFFFu);  // Outline around selected notes
const juce::Colour APP_COLOR_NOTE_BOUNDARY_GUIDE      = juce::Colour(0xFF9A98A4u);  // Split / boundary guide lines between notes
const juce::Colour APP_COLOR_VALUE_BADGE              = juce::Colour(0xFF2E2D34u);  // Badge showing a dragged value (st, dB)
const juce::Colour APP_COLOR_VALUE_BADGE_TEXT         = juce::Colour(0xFFEFEBF9u);  // Text on VALUE_BADGE
const juce::Colour APP_COLOR_HOVER_LABEL              = juce::Colour(0xE628272Du);  // Hover info label fill
const juce::Colour APP_COLOR_HOVER_LABEL_TEXT         = juce::Colour(0xFFF2EEFCu);  // Hover info label text
const juce::Colour APP_COLOR_BREATH                   = juce::Colour(0xFF7FC8FFu);  // Breath capsules
const juce::Colour APP_COLOR_SIBILANCE                = juce::Colour(0xFFFFC46Bu);  // Sibilance capsules
const juce::Colour APP_COLOR_NOISE_HATCH              = juce::Colour(0xFF000000u);  // Hatching over noisy spans in notes
const juce::Colour APP_COLOR_PITCH_HANDLE             = juce::Colour(0xFF2E2D34u);  // Pitch tool handle
const juce::Colour APP_COLOR_PITCH_HANDLE_HOVER       = juce::Colour(0xFF3B3A42u);  // Pitch tool handle, hovered
const juce::Colour APP_COLOR_PITCH_HANDLE_TILT        = juce::Colour(0xFFFFA500u);  // Tilt handle accent
const juce::Colour APP_COLOR_PITCH_HANDLE_DRIFT       = juce::Colour(0xFF9370DBu);  // Drift / fine pitch / vibrato handle accent
const juce::Colour APP_COLOR_PITCH_HANDLE_SMOOTH      = juce::Colour(0xFF00FFFFu);  // Smooth handle accent
const juce::Colour APP_COLOR_PITCH_HANDLE_DEFAULT     = juce::Colour(0xFFFFFFFFu);  // Other handle accents

// ── Scale accents ─────────────────────────────────────────────────
const juce::Colour APP_COLOR_SCALE_MAJOR              = juce::Colour(0xFF74A9FFu);  // Major
const juce::Colour APP_COLOR_SCALE_MINOR              = juce::Colour(0xFFB689FFu);  // Minor
const juce::Colour APP_COLOR_SCALE_DORIAN             = juce::Colour(0xFF5BD0C0u);  // Dorian
const juce::Colour APP_COLOR_SCALE_PHRYGIAN           = juce::Colour(0xFFFF8C77u);  // Phrygian
const juce::Colour APP_COLOR_SCALE_LYDIAN             = juce::Colour(0xFFFFD166u);  // Lydian
const juce::Colour APP_COLOR_SCALE_MIXOLYDIAN         = juce::Colour(0xFF75D0FFu);  // Mixolydian
const juce::Colour APP_COLOR_SCALE_LOCRIAN            = juce::Colour(0xFF9FA9BFu);  // Locrian
const juce::Colour APP_COLOR_SCALE_BLUES              = juce::Colour(0xFF4F86C6u);  // Blues
const juce::Colour APP_COLOR_SCALE_HARMONIC_MINOR     = juce::Colour(0xFFC27BA0u);  // Harmonic minor
const juce::Colour APP_COLOR_SCALE_MAJOR_PENTATONIC   = juce::Colour(0xFFF2C14Eu);  // Major pentatonic
const juce::Colour APP_COLOR_SCALE_MELODIC_MINOR      = juce::Colour(0xFFA78BFAu);  // Melodic minor
const juce::Colour APP_COLOR_SCALE_MINOR_PENTATONIC   = juce::Colour(0xFF8F7CC3u);  // Minor pentatonic
const juce::Colour APP_COLOR_SCALE_PHRYGIAN_DOMINANT  = juce::Colour(0xFFE07A5Fu);  // Phrygian dominant
const juce::Colour APP_COLOR_SCALE_WHOLE_TONE         = juce::Colour(0xFF81B29Au);  // Whole tone

// ── Overview ──────────────────────────────────────────────────────
const juce::Colour APP_COLOR_OVERVIEW_WAVEFORM        = juce::Colour(0xFF46454Eu);  // Overview waveform
const juce::Colour APP_COLOR_OVERVIEW_VIEWPORT        = juce::Colour(0xFFEFEBF9u);  // Visible-range box in the overview
const juce::Colour APP_COLOR_OVERVIEW_PLAYHEAD        = juce::Colour(0xFFC7C4D1u);  // Overview playhead

// ── Toggles & radio buttons ───────────────────────────────────────
const juce::Colour APP_COLOR_TOGGLE_FILL              = juce::Colour(0xFF565560u);  // Plain toggle button fill
const juce::Colour APP_COLOR_TOGGLE_FILL_HOVER        = juce::Colour(0xFF72707Cu);  // Plain toggle button fill, hovered
const juce::Colour APP_COLOR_TOGGLE_TEXT_ON           = juce::Colour(0xFFE6E3F0u);  // Plain toggle button label, on
const juce::Colour APP_COLOR_TOGGLE_TEXT_OFF          = juce::Colour(0xFF9B99A5u);  // Plain toggle button label, off
const juce::Colour APP_COLOR_RADIO_HOVER              = juce::Colour(0xFF9B99A5u);  // Radio button ring, hovered
const juce::Colour APP_COLOR_RADIO_IDLE               = juce::Colour(0xFF51505Bu);  // Radio button ring, idle
const juce::Colour APP_COLOR_RADIO_LABEL_HOVER        = juce::Colour(0xFF9B99A5u);  // Radio button label, hovered
const juce::Colour APP_COLOR_RADIO_LABEL_IDLE         = juce::Colour(0xFF51505Bu);  // Radio button label, idle

// ── Note pitch gradient (in tune -> out of tune) ──────────────────
const juce::Colour APP_COLOR_NOTE_IN_TUNE_CENTRE      = juce::Colour(0xFFB3A4FFu);  // In-tune note, centre
const juce::Colour APP_COLOR_NOTE_IN_TUNE_SIDE        = juce::Colour(0xFF8E7CF0u);  // In-tune note, edges
const juce::Colour APP_COLOR_NOTE_CAUTION_CENTRE      = juce::Colour(0xFFF29AC8u);  // Note off by the caution threshold, centre
const juce::Colour APP_COLOR_NOTE_CAUTION_SIDE        = juce::Colour(0xFFDB72AEu);  // Note off by the caution threshold, edges
const juce::Colour APP_COLOR_NOTE_OUT_OF_TUNE_CENTRE  = juce::Colour(0xFFFF9F5Au);  // Out-of-tune note, centre
const juce::Colour APP_COLOR_NOTE_OUT_OF_TUNE_SIDE    = juce::Colour(0xFFF07A2Au);  // Out-of-tune note, edges

// ── Track list ────────────────────────────────────────────────────
const juce::Colour APP_COLOR_TRACK_DEFAULT            = juce::Colour(0xFFE6E3F0u);  // Track colour when the DAW sends none, or "Use DAW Track Color" is off
const juce::Colour APP_COLOR_TRACK_ROW_ACTIVE         = juce::Colour(0xFF35343Bu);  // Active track row fill
const juce::Colour APP_COLOR_TRACK_NAME_INACTIVE      = juce::Colour(0xFF9A98A4u);  // Inactive track name

// ── Poly lab ──────────────────────────────────────────────────────
const juce::Colour APP_COLOR_POLYLAB_BLACK_KEY_ROW    = juce::Colour(0xFF11141Bu);  // Black-key rows
const juce::Colour APP_COLOR_POLYLAB_EDITED_CURVE     = juce::Colour(0xFF08262Au);  // Pitch curve of moved notes

// ── Scrollbars ────────────────────────────────────────────────────
const juce::Colour APP_COLOR_SCROLLBAR_THUMB      = juce::Colour(0xFF565560u);  // Thumb
const juce::Colour APP_COLOR_SCROLLBAR_TRACK      = juce::Colour(0xFF0C0C0Du);  // Recessed gutter

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
