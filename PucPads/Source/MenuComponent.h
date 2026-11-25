#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
    Menu introdutorio: porta de entrada do app.

    A partir daqui o usuario vai para a tela unica de performance (LaunchPad)
    ou para o Mixer & Configuracoes.
*/
class MenuComponent : public juce::Component
{
public:
    MenuComponent();
    ~MenuComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    // Entrada para a tela de LaunchPad (unica tela de performance do app)
    juce::TextButton launchPadButton;

    // Acesso ao Mixer & Configuracoes
    juce::TextButton settingsButton;

private:
    static constexpr int titleAreaHeight = 190;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MenuComponent)
};
