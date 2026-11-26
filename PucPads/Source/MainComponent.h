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

    // Mixer: ganho master linear (0..1), aplicado no audio thread com rampa curta.
    void setMasterGain(float linearGain);
    float getMasterGain() const noexcept { return masterGain.load(); }

    // Volume da base musical (0..1): repassado ao LaunchPad/template ativo.
    void setBackingGain(float linearGain);
    float getBackingGain() const noexcept { return backingGain; }

    // Funções de Navegação
    void showLaunchPadScreen(); // Tela unica de performance (pads + templates)
    void showSettingsScreen();  // Mixer & Configuracoes
    void showMenuScreen();      // Volta para o menu introdutorio

    void buttonClicked(juce::Button* button) override;

private:
    juce::MixerAudioSource mixerSource;

    // Ganho master: valor definido pela UI (lido pelo audio thread) e rampa
    // usada para evitar estalo ao mover o slider.
    std::atomic<float> masterGain { 1.0f };
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> masterGainRamp;

    // Ganho da base musical, guardado para reaplicar em cada tela de LaunchPad
    // (a tela e recriada a cada entrada no modo performance).
    float backingGain = 1.0f;

    // Nossas tres "telas" gerenciadas com unique_ptr
    std::unique_ptr<MenuComponent> menuScreen;
    std::unique_ptr<LaunchPadComponent> launchPadScreen;
    std::unique_ptr<SettingsComponent> settingsScreen;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
