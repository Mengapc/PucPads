#include <JuceHeader.h>
#include "PadComponent.h"

//==============================================================================
PadComponent::PadComponent(const char* soundData, int soundDataSize, juce::MixerAudioSource& mixerToUse, juce::Colour padColour)
    : mixerRef(mixerToUse)
    
{
    activeColour = padColour;
    setupAudio(soundData, soundDataSize, mixerToUse);
}

PadComponent::~PadComponent()
{
    stopTimer();

    mixerRef.removeInputSource(&transportSource);
}

void PadComponent::paint (juce::Graphics& g)
{
    juce::Colour corFundoInativo = juce::Colour::fromString("ff420012");
    juce::Colour corBordaInativa = juce::Colour::fromString("ffb08d35");

    // A MÁGICA DA COR: Mistura o fundo inativo com a cor Neon baseada no "currentFade"
    juce::Colour corAtual = corFundoInativo.interpolatedWith(activeColour, currentFade);

    g.fillAll(corAtual);

    // Se o brilho estiver alto (> 0.8), a borda fica branca (estouro de luz)
    // Se estiver diminuindo, a borda volta a ser dourada
    g.setColour(currentFade > 0.8f ? juce::Colours::white : corBordaInativa);

    g.drawRoundedRectangle(getLocalBounds().reduced(1).toFloat(), 4.0f, 1.5f);
}

void PadComponent::resized()
{
}

void PadComponent::mouseDown (const juce::MouseEvent& event)
{
    isMouseDown = true;
    
    // Força o brilho para o máximo instantaneamente
    currentFade = 1.0f;
    stopTimer();

    transportSource.setPosition(0); // Rebobina o som para o início
    transportSource.start();          // Toca o som
    
    repaint();
}

void PadComponent::mouseUp (const juce::MouseEvent& event)
{
    isMouseDown = false;
    
    // Inicia a animação a 60 quadros por segundo (60 Hz)
    startTimerHz(60);
    
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

void PadComponent::timerCallback()
{
    // Reduz o brilho (0.05 por frame = fade rápido. Mude para 0.02 para fade lento)
    currentFade -= 0.05f;

    if (currentFade <= 0.0f)
    {
        currentFade = 0.0f; // Trava no zero
        stopTimer();        // Desliga o timer para economizar CPU
    }

    repaint(); // Pede para desenhar a nova cor intermediária
}