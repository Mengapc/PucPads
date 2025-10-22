#include <JuceHeader.h>
#include "MenuComponent.h"

//==============================================================================
MenuComponent::MenuComponent()
{
    startButton.setButtonText("Iniciar PucPads");
    startButton.setColour(juce::TextButton::buttonColourId, juce::Colours::red);
    addAndMakeVisible(startButton);
}

MenuComponent::~MenuComponent()
{
}

void MenuComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);
}

void MenuComponent::resized()
{
    startButton.setBounds(getLocalBounds().reduced(50).withSizeKeepingCentre(200, 50));
}