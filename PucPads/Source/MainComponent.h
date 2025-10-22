#pragma once

#include <JuceHeader.h>
#include "PadGridComponent.h" // Inclui nossa nova tela de pads
#include "MenuComponent.h"    // Inclui nossa nova tela de menu

//==============================================================================
// MainComponent agora é o Gerenciador de Áudio e "Ouvinte" de Botões
class MainComponent : public juce::AudioAppComponent,
    public juce::Button::Listener // Para ouvir o startButton
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

    // Função que será chamada quando um botão for clicado
    void buttonClicked(juce::Button* button) override;

private:
    // O mixer agora vive aqui, no topo
    juce::MixerAudioSource mixerSource;

    // Nossas duas "telas"
    // Usamos unique_ptr para gerenciar a memória delas
    std::unique_ptr<MenuComponent> menuScreen;
    std::unique_ptr<PadGridComponent> padGridScreen;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};