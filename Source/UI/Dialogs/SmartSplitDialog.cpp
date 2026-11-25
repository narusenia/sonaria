#include "SmartSplitDialog.h"
#include "../Components/AppFont.h"
#include "../Components/DarkLookAndFeel.h"
#include "../Components/StyledWidgets.h"
#include "../../Utils/Localization.h"
#include "../../Utils/UI/Theme.h"
#include <utility>

namespace SmartSplitDialog {
namespace {

// Same frame and buttons as the Pitch Center popup (QuantizePitchDialog).
class Content final : public juce::Component
{
public:
  Content(std::function<void(float)> preview, std::function<void(bool, float)> completed)
      : onPreview(std::move(preview)), onComplete(std::move(completed))
  {
    sensitivityLabel.setText(TR("smart_split.sensitivity"), juce::dontSendNotification);
    sensitivityLabel.setFont(AppFont::getFont(15.0f));
    sensitivityLabel.setColour(juce::Label::textColourId, APP_COLOR_TEXT_PRIMARY);
    sensitivity.setRange(0.0, 100.0, 1.0);
    sensitivity.setTextValueSuffix(" %");
    sensitivity.setTextBoxStyle(juce::Slider::TextBoxRight, true, 64, 22);
    sensitivity.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colours::transparentBlack);
    sensitivity.setValue(50.0, juce::dontSendNotification);
    sensitivity.onValueChange = [this]
    {
      if (!finished && onPreview)
        onPreview(value());
    };

    setupButton(cancelButton, TR("dialog.cancel"));
    setupButton(okButton, TR("dialog.ok"));
    cancelButton.onClick = [this] { finish(false); };
    okButton.onClick = [this] { finish(true); };

    addAndMakeVisible(sensitivityLabel);
    addAndMakeVisible(sensitivity);
    addAndMakeVisible(cancelButton);
    addAndMakeVisible(okButton);
  }

  ~Content() override
  {
    if (!finished && onComplete)
      onComplete(false, value());
  }

  void start()
  {
    if (onPreview)
      onPreview(value());
  }

  void cancel() { finish(false); }

  void paint(juce::Graphics &g) override
  {
    g.setColour(APP_COLOR_POPOVER_BACKGROUND);
    g.fillRoundedRectangle(getLocalBounds().toFloat(), 10.0f);
    g.setColour(APP_COLOR_BORDER.withAlpha(0.55f));
    g.drawRoundedRectangle(getLocalBounds().toFloat().reduced(0.5f), 10.0f, 1.0f);
  }

  void resized() override
  {
    auto area = getLocalBounds().reduced(16, 10);
    auto row = area.removeFromTop(32);
    sensitivityLabel.setBounds(row.removeFromLeft(94));
    sensitivity.setBounds(row);
    area.removeFromTop(8);
    auto buttons = area.removeFromTop(24).removeFromRight(160).translated(1, 0);
    cancelButton.setBounds(buttons.removeFromLeft(75));
    buttons.removeFromLeft(10);
    okButton.setBounds(buttons);
  }

private:
  static void setupButton(juce::TextButton &button, const juce::String &text)
  {
    button.setButtonText(text);
    button.setLookAndFeel(&DarkLookAndFeel::getInstance());
    button.setColour(juce::TextButton::buttonColourId, APP_COLOR_BUTTON_FILL);
    button.setColour(juce::TextButton::buttonOnColourId, APP_COLOR_BUTTON_FILL_DOWN);
    button.setColour(juce::TextButton::textColourOffId, APP_COLOR_BUTTON_TEXT);
    button.setMouseCursor(juce::MouseCursor::PointingHandCursor);
  }

  float value() const { return static_cast<float>(sensitivity.getValue() / 100.0); }

  void finish(bool accepted)
  {
    if (finished)
      return;
    finished = true;
    if (onComplete)
      onComplete(accepted, value());
    auto safeThis = juce::Component::SafePointer<Content>(this);
    juce::MessageManager::callAsync([safeThis]
    {
      if (auto *content = safeThis.getComponent())
      {
        if (auto *parent = content->getParentComponent())
          parent->removeChildComponent(content);
        delete content;
      }
    });
  }

  std::function<void(float)> onPreview;
  std::function<void(bool, float)> onComplete;
  juce::Label sensitivityLabel;
  MacroSlider sensitivity;
  juce::TextButton cancelButton, okButton;
  bool finished = false;
};

juce::Component::SafePointer<Content> activePopup;
} // namespace

void dismissPopup()
{
  if (auto *content = activePopup.getComponent())
    content->cancel();
}

void showPopup(juce::Component *parent, juce::Rectangle<int> anchorBounds,
               std::function<void(float)> onPreview,
               std::function<void(bool, float)> onComplete)
{
  if (parent == nullptr)
    return;

  dismissPopup();

  constexpr int width = 330;
  constexpr int height = 84;
  auto *content = new Content(std::move(onPreview), std::move(onComplete));
  content->setSize(width, height);
  const auto bounds = parent->getLocalBounds();
  const int x = juce::jlimit(8, juce::jmax(8, bounds.getWidth() - width - 8),
                             anchorBounds.getRight() - width);
  const int y = juce::jlimit(0, juce::jmax(0, bounds.getHeight() - height),
                             anchorBounds.getY());
  parent->addAndMakeVisible(content);
  content->setBounds(x, y, width, height);
  content->toFront(false);
  activePopup = content;
  content->start();
}

} // namespace SmartSplitDialog
