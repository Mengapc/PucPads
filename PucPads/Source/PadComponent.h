#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
    Esta classe representa um único pad na nossa interface.
*/
class PadComponent  : public juce::Component, public juce::Timer
{
public:
    PadComponent(const char* soundData, int soundDataSize, juce::MixerAudioSource& mixerToUse, juce::Colour padColour);
    ~PadComponent() override;

    juce::AudioTransportSource& getTransportSource() { return transportSource; }
    
    void paint (juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

    // Funções de evento do mouse
    void mouseDown (const juce::MouseEvent& event) override;
    void mouseUp (const juce::MouseEvent& event) override;
    void mouseExit(const juce::MouseEvent& event) override;
    
private:
    juce::MixerAudioSource& mixerRef;
    
    //Variavel para guardar estado do mouse
    bool isMouseDown = false;

    // Variável para controle de animação do brilho
    float currentFade = 0.0f;

    // Variável para guardar a cor
    juce::Colour activeColour;

    void setupAudio(const char* soundData, int soundDataSize, juce::MixerAudioSource& mixerToUse); // Função para organizar o carregamento do som
    
    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transportSource;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PadComponent)
};