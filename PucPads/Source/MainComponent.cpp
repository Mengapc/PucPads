#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    // Configurações iniciais de áudio
    setAudioChannels (0, 2); // 0 canais de entrada, 2 de saída (estéreo)
    
    // Adiciona o pad na tela
    addAndMakeVisible(myPad);
    
    setSize (600, 400);
}

MainComponent::~MainComponent()
{
    // Libera os recurso de áudio ao fechar
    shutdownAudio();
}

//================== Funções de áudio
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    // Passagem de som
    myPad.getTransportSource().prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    // Requisição de preenchimento de buffer para o próximo pedaço de som
    myPad.getTransportSource().getNextAudioBlock(bufferToFill);
}

void MainComponent::releaseResources()
{
    // Quando o áudio para, liberamos os recursos do audioplayer
    myPad.getTransportSource().releaseResources();
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setFont (juce::FontOptions (16.0f));
    g.setColour (juce::Colours::white);
    g.drawText ("Hello World!", getLocalBounds(), juce::Justification::centred, true);
}

void MainComponent::resized()
{
    myPad.setBounds(10, 10, 100, 100);
}
