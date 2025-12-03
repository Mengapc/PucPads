#include <JuceHeader.h>
#include "PadComponent.h"

//==============================================================================
PadComponent::PadComponent(const char* soundData, int soundDataSize, juce::MixerAudioSource& mixerToUse, juce::Colour padColour)
    : activeColour(padColour) // Inicializa a nossa variável de cor
{
    setupAudio(soundData, soundDataSize, mixerToUse);
}

PadComponent::~PadComponent()
{
}

void PadComponent::paint (juce::Graphics& g)
{
    // Lógica principal: Decidir qual cor usar AGORA
    juce::Colour corParaPintar;

    if (isMouseDown)
    {
        // Se estiver apertado: Usa a cor ORIGINAL (brilhante)
        corParaPintar = activeColour;
    }
    else
    {
        // Se estiver solto: Usa a cor ESCURECIDA
        corParaPintar = juce::Colours::black;
    }

    g.fillAll(corParaPintar);
    // Desenha uma borda arredondada bonita
    g.setColour(juce::Colours::black.withAlpha(0.5f)); // Borda preta semi-transparente
    g.drawRect(getLocalBounds(), 1);
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

void PadComponent::mouseExit(const juce::MouseEvent& event)
{
    if (isMouseDown)
    {
        isMouseDown = false;
        // transportSource.stop(); // Se usar modo Gate
        repaint();
    }
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