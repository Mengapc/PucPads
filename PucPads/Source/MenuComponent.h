#pragma once

#include <JuceHeader.h>

//==============================================================================
class MenuComponent : public juce::Component
{
public:
    MenuComponent();
    ~MenuComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;

    // Tornamos o botão público para que o MainComponent possa "ouvi-lo"
    juce::TextButton startButton;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MenuComponent)
};