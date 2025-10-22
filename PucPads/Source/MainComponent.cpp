#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    // 1. Cria nossa tela de pads e passa o mixer para ela
    padGridScreen = std::make_unique<PadGridComponent>(mixerSource);
    // 2. Cria nossa tela de menu
    menuScreen = std::make_unique<MenuComponent>();
    // Registra o MainComponent como "ouvinte" do botão "start"
    menuScreen->startButton.addListener(this);
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
    menuScreen->startButton.removeListener(this);
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
    if (menuScreen != nullptr)
        menuScreen->setBounds(getLocalBounds());

    if (padGridScreen != nullptr)
        padGridScreen->setBounds(getLocalBounds());
}

//================== A LÓGICA DE NAVEGAÇÃO ==================
void MainComponent::buttonClicked(juce::Button* button)
{
    // Verifica se o botão clicado foi o nosso "startButton"
    if (button == &menuScreen->startButton)
    {
        // 1. Esconde e remove o menu
        menuScreen->setVisible(false);
        removeChildComponent(menuScreen.get());

        // 2. Adiciona e torna a tela de pads visível
        addAndMakeVisible(*padGridScreen);

        // 3. (Opcional) Força o padGrid a se redimensionar para
        // caber na tela, ativando sua função resized()
        padGridScreen->setBounds(getLocalBounds());
    }
}