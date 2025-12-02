#include <JuceHeader.h>
#include "PadComponent.h"

//==============================================================================
PadComponent::PadComponent(const char* soundData, int soundDataSize, juce::MixerAudioSource& mixerToUse)
{
    setupAudio(soundData, soundDataSize, mixerToUse);
}

PadComponent::~PadComponent()
{
}

void PadComponent::paint (juce::Graphics& g)
{
    if (isMouseDown)
    {
        g.fillAll (juce::Colours::yellow); // Cor Pressionado
    }
    else
    {
        g.fillAll (juce::Colours::black);  // Cor Não Pressionado  
    }
    
    
    // Borda do pad
    g.setColour (juce::Colours::white);
    g.drawRect (getLocalBounds(), 1); // getLocalBounds() pega o retângulo inteiro do componente.
}

void PadComponent::resized()
{
}

void PadComponent::mouseDown (const juce::MouseEvent& event)
{
    isMouseDown = true;
    
    transportSource.setPosition(0); // Rebobina o som para o início
    transportSource.start();          // Toca o som
    
    repaint();
}

void PadComponent::mouseUp (const juce::MouseEvent& event)
{
    isMouseDown = false;
    transportSource.stop();
    repaint();
}


void PadComponent::setupAudio(const char* soundData, int soundDataSize, juce::MixerAudioSource& mixerToUse)
{
    // 1. Registrar os formatos de áudio
    formatManager.registerBasicFormats();
    
    // 2. Criar um "leitor" para o som a partir da memória
    auto* reader = formatManager.createReaderFor(std::make_unique<juce::MemoryInputStream>(soundData, soundDataSize, false));
    
    if (reader != nullptr)
    {
        // 3. Conectar o leitor ao "toca-fitas"
        std::unique_ptr<juce::AudioFormatReaderSource> newSource (new juce::AudioFormatReaderSource (reader, true));
        transportSource.setSource (newSource.get(), 0, nullptr, reader->sampleRate);
        readerSource.reset (newSource.release());
		mixerToUse.addInputSource(&transportSource, false);
    }
    else
    {
        DBG("ERRO: Falha ao carregar o arquivo de áudio!");
    }
}