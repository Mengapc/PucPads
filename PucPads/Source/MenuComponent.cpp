#include <JuceHeader.h>
#include "MenuComponent.h"

//==============================================================================
MenuComponent::MenuComponent()
{
    // Configura o primeiro botão
    kitMelodiaButton.setButtonText(juce::String::fromUTF8("Kit Melodia (Dó-Ré-Mi)"));
    addAndMakeVisible(kitMelodiaButton);

    // Configura o segundo botão
    kitBateriaButton.setButtonText("Kit Bateria");
    addAndMakeVisible(kitBateriaButton);
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
    // Posiciona os botões um abaixo do outro
    auto bounds = getLocalBounds().reduced(50); // Uma margem de 50

    kitMelodiaButton.setBounds(bounds.removeFromTop(50));
    bounds.removeFromTop(20); // Um espaço de 20px entre os botões
    kitBateriaButton.setBounds(bounds.removeFromTop(50));
}