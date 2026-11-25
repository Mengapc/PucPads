#include <JuceHeader.h>
#include "MenuComponent.h"
#include "UiTheme.h"

//==============================================================================
MenuComponent::MenuComponent()
{
    // --- ESTÉTICA INSTITUCIONAL ---

    // Botao principal: leva a tela unica de performance
    launchPadButton.setButtonText("LAUNCH PAD");
    UiTheme::applyButtonStyle(launchPadButton);
    launchPadButton.setColour(juce::TextButton::textColourOffId, UiTheme::dourado());
    addAndMakeVisible(launchPadButton);

    // --- BOTAO DE CONFIGURACOES ---
    settingsButton.setButtonText(juce::String::fromUTF8("Mixer & Configurações"));
    UiTheme::applyButtonStyle(settingsButton);
    addAndMakeVisible(settingsButton);
}

MenuComponent::~MenuComponent()
{
}

void MenuComponent::paint(juce::Graphics& g)
{
    // Fundo Bordo escuro para manter a consistência com a tela de Pads
    g.fillAll(UiTheme::bordoEscuro());

    // Area de titulo (o espaco correspondente e reservado em resized())
    auto titleArea = getLocalBounds().removeFromTop(titleAreaHeight).reduced(20);

    g.setColour(UiTheme::dourado());
    g.setFont(juce::Font(juce::FontOptions(38.0f).withStyle("Bold")));
    g.drawText("PUCPADS", titleArea.removeFromTop(60), juce::Justification::centredBottom, false);

    g.setColour(UiTheme::dourado().withAlpha(0.65f));
    g.setFont(juce::Font(juce::FontOptions(15.0f)));
    g.drawText(juce::String::fromUTF8("Launch pad musical"),
               titleArea.removeFromTop(30), juce::Justification::centredTop, false);
}

void MenuComponent::resized()
{
    auto bounds = getLocalBounds().reduced(40);

    // Espaco do titulo (desenhado em paint())
    bounds.removeFromTop(titleAreaHeight - 40);

    // Botao principal em destaque, depois o acesso ao Mixer
    launchPadButton.setBounds(bounds.removeFromTop(90));
    bounds.removeFromTop(20);
    settingsButton.setBounds(bounds.removeFromTop(50));
}
