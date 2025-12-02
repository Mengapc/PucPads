#pragma once

#include <JuceHeader.h>
#include "PadComponent.h"

//==============================================================================
class PadGridComponent : public juce::Component
{
public:
    // Nosso construtor vai receber o mixer principal
    PadGridComponent(juce::MixerAudioSource& mixerToUse, juce::String kitFileToLoad);
    ~PadGridComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    // O array de pads agora vive aqui!
    juce::OwnedArray<PadComponent> pads;

    // O mixer 	ma referencia ao mixer que est no MainComponent
    juce::MixerAudioSource& mixerSource;

    juce::AudioFormatManager formatManager; // Gerenciador de formatos (mp3, wav)
    std::unique_ptr<juce::AudioFormatReaderSource> backgroundReaderSource;
    juce::AudioTransportSource backgroundSource; // O "Player" da base

    // Botões de controle
    juce::TextButton playButton{ "Play Base" };
    juce::TextButton stopButton{ "Stop Base" };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PadGridComponent)
};