#include <JuceHeader.h>
#include "PadComponent.h"

//==============================================================================
PadComponent::PadComponent()
{
    setupAudio();
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
    repaint();
}


void PadComponent::setupAudio()
{
    // 1. Registrar os formatos de áudio
    formatManager.registerBasicFormats();
    
    // 2. Acessar nosso som que o Projucer converteu
    auto* audioAsset = BinaryData::C_mp3;
    int audioAssetSize = BinaryData::C_mp3Size;
    
    // 3. Criar um "leitor" para o som a partir da memória
    auto* reader = formatManager.createReaderFor(std::make_unique<juce::MemoryInputStream>(audioAsset, audioAssetSize, false));
    
    if (reader != nullptr)
    {
        // 4. Conectar o leitor ao "toca-fitas"
        std::unique_ptr<juce::AudioFormatReaderSource> newSource (new juce::AudioFormatReaderSource (reader, true));
        transportSource.setSource (newSource.get(), 0, nullptr, reader->sampleRate);
        readerSource.reset (newSource.release());
    }
    else
    {
        DBG("ERRO: Falha ao carregar o arquivo de áudio!");
    }
}