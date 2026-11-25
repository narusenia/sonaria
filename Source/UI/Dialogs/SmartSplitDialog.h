#pragma once

#include "../../JuceHeader.h"
#include <functional>

namespace SmartSplitDialog {

// A popup with one sensitivity slider (0..1, default 0.5). onPreview gets
// every value, including the initial one; onComplete gets the final value
// and true only when the user pressed OK (Cancel, Escape or another popup
// give false).
void showPopup(juce::Component *parent, juce::Rectangle<int> anchorBounds,
               std::function<void(float)> onPreview,
               std::function<void(bool, float)> onComplete);

void dismissPopup();

} // namespace SmartSplitDialog
