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

    juce::TextButton kitMelodiaButton;
    juce::TextButton kitBateriaButton;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MenuComponent)
};