#include <JuceHeader.h>
#include "MenuComponent.h"

//==============================================================================
MenuComponent::MenuComponent()
{
    // --- ESTÉTICA INSTITUCIONAL ---
    juce::Colour corFundoBotao = juce::Colour::fromString("ff420012"); // Bordô
    juce::Colour corTextoBotao = juce::Colour::fromString("ffffd700"); // Dourado Neon

    // Configura o primeiro botão (Kit Melodia)
    kitMelodiaButton.setButtonText(juce::String::fromUTF8("Kit Melodia (Dó-Ré-Mi)"));
    kitMelodiaButton.setColour(juce::TextButton::buttonColourId, corFundoBotao);
    kitMelodiaButton.setColour(juce::TextButton::textColourOffId, corTextoBotao);
    addAndMakeVisible(kitMelodiaButton);

    // Configura o segundo botão (Kit Bateria)
    kitBateriaButton.setButtonText("Kit Bateria");
    kitBateriaButton.setColour(juce::TextButton::buttonColourId, corFundoBotao);
    kitBateriaButton.setColour(juce::TextButton::textColourOffId, corTextoBotao);
    addAndMakeVisible(kitBateriaButton);

    // --- NOVO BOTÃO DE CONFIGURAÇÕES ---
    settingsButton.setButtonText(juce::String::fromUTF8("Mixer & Configurações"));
    settingsButton.setColour(juce::TextButton::buttonColourId, corFundoBotao);
    settingsButton.setColour(juce::TextButton::textColourOffId, corTextoBotao);
    addAndMakeVisible(settingsButton);
}

MenuComponent::~MenuComponent()
{
}

void MenuComponent::paint(juce::Graphics& g)
{
    // Fundo Bordô escuro para manter a consistência com a tela de Pads
    g.fillAll(juce::Colour::fromString("ff2a000b"));
}

void MenuComponent::resized()
{
    // Posiciona os botões um abaixo do outro
    auto bounds = getLocalBounds().reduced(50); // Uma margem de 50

    kitMelodiaButton.setBounds(bounds.removeFromTop(50));
    bounds.removeFromTop(20); // Um espaço de 20px entre os botões
    
    kitBateriaButton.setBounds(bounds.removeFromTop(50));
    bounds.removeFromTop(20); // Um espaço de 20px entre os botões
    
    // Posiciona o novo botão
    settingsButton.setBounds(bounds.removeFromTop(50));
}