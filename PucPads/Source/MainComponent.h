#pragma once

#include <JuceHeader.h>
#include "LaunchPadComponent.h"
#include "MenuComponent.h"
#include "SettingsComponent.h"

//==============================================================================
class MainComponent : public juce::AudioAppComponent,
    public juce::Button::Listener
{
public:
    MainComponent();
    ~MainComponent() override;

    // Funções de Áudio
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    // Funções de UI
    void paint(juce::Graphics&) override;
    void resized() override;

    // Funções de Navegação
    void showLaunchPadScreen(); // Tela unica de performance (pads + templates)
    void showSettingsScreen();  // Mixer & Configuracoes
    void showMenuScreen();      // Volta para o menu introdutorio

    void buttonClicked(juce::Button* button) override;

private:
    juce::MixerAudioSource mixerSource;

    // Nossas tres "telas" gerenciadas com unique_ptr
    std::unique_ptr<MenuComponent> menuScreen;
    std::unique_ptr<LaunchPadComponent> launchPadScreen;
    std::unique_ptr<SettingsComponent> settingsScreen;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
