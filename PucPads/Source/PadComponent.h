#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
    Esta classe representa um único pad na nossa interface.
*/
class PadComponent  : public juce::Component // <<<<<<< A CORREÇÃO ESTÁ AQUI
{
public:
    PadComponent();
    ~PadComponent() override;

    juce::AudioTransportSource& getTransportSource() { return transportSource; }
    
    void paint (juce::Graphics&) override;
    void resized() override;

    // Funções de evento do mouse
    void mouseDown (const juce::MouseEvent& event) override;
    void mouseUp (const juce::MouseEvent& event) override;
    
private:
    //Variavel para guardar estado do mouse
    bool isMouseDown = false;

    void setupAudio(); // Função para organizar o carregamento do som
    
    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transportSource;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PadComponent)
};