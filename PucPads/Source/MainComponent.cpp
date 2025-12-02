#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    // Cria nossa tela de menu
    menuScreen = std::make_unique<MenuComponent>();
    // Registra o MainComponent como "ouvinte" dos botões
    menuScreen->kitMelodiaButton.addListener(this);
    menuScreen->kitBateriaButton.addListener(this);
    // Adiciona e torna o menu visível por padrão
    addAndMakeVisible(*menuScreen);

    // Configura o áudio
    setAudioChannels(0, 2);
    
    // Configura o tamanho da janela
    setSize(450, 700);
}

MainComponent::~MainComponent()
{
    // Desliga o áudio e limpa os listeners
    shutdownAudio();
    menuScreen->kitMelodiaButton.removeListener(this);
    menuScreen->kitBateriaButton.removeListener(this);
}

//================== Funções de Áudio (continuam iguais)
void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    mixerSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    mixerSource.getNextAudioBlock(bufferToFill);
}

void MainComponent::releaseResources()
{
    mixerSource.releaseResources();
}

//==============================================================================
void MainComponent::paint(juce::Graphics& g)
{
    // O fundo do MainComponent (o "gerenciador")
    g.fillAll(juce::Colours::black);
}

void MainComponent::resized()
{
    // Esta função agora posiciona qualquer tela que esteja visível
    // para preencher a janela inteira.
    if (menuScreen != nullptr && menuScreen->isVisible())
        menuScreen->setBounds(getLocalBounds());

    if (padGridScreen != nullptr && menuScreen->isVisible())
        padGridScreen->setBounds(getLocalBounds());
}

//================== A LÓGICA DE NAVEGAÇÃO ==================

// Uma nova função para encapsular a lógica de troca de tela
void MainComponent::showPadScreen(juce::String kitFile)
{
    // 1. Esconde e remove o menu
    menuScreen->setVisible(false);
    removeChildComponent(menuScreen.get());

    // 2. CRIA a tela de pads, passando o nome do kit
    padGridScreen = std::make_unique<PadGridComponent>(mixerSource, kitFile);

    // 3. Adiciona e torna a tela de pads visível
    addAndMakeVisible(*padGridScreen);
    padGridScreen->setBounds(getLocalBounds());
}

void MainComponent::buttonClicked(juce::Button* button)
{
    // Verifica se o botão clicado foi o nosso "startButton"
    if (button == &menuScreen->kitMelodiaButton)
    {
        showPadScreen("notes_json");
    }
    else if (button == &menuScreen->kitBateriaButton)
    {
        showPadScreen("drumkit_json");
    }
}