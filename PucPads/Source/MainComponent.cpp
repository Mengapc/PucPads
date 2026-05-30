#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    menuScreen = std::make_unique<MenuComponent>();
    
    menuScreen->kitMelodiaButton.addListener(this);
    menuScreen->kitBateriaButton.addListener(this);
    menuScreen->settingsButton.addListener(this);
    
    addAndMakeVisible(*menuScreen);

    setAudioChannels(0, 2);
    setSize(450, 700);
}

MainComponent::~MainComponent()
{
    shutdownAudio();
    menuScreen->kitMelodiaButton.removeListener(this);
    menuScreen->kitBateriaButton.removeListener(this);
    menuScreen->settingsButton.removeListener(this);
    
    if (settingsScreen != nullptr)
        settingsScreen->backButton.removeListener(this);

    if (padGridScreen != nullptr)
        padGridScreen->backButton.removeListener(this);
}

//================== Funções de Áudio (continuam iguais) ==================
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
    g.fillAll(juce::Colours::black);
}

void MainComponent::resized()
{
    if (menuScreen != nullptr && menuScreen->isVisible())
        menuScreen->setBounds(getLocalBounds());

    // CORREÇÃO: Estava menuScreen->isVisible() aqui antes!
    if (padGridScreen != nullptr && padGridScreen->isVisible()) 
        padGridScreen->setBounds(getLocalBounds());
        
    // Redimensiona o Mixer caso ele seja a tela ativa
    if (settingsScreen != nullptr && settingsScreen->isVisible())
        settingsScreen->setBounds(getLocalBounds());
}

//================== A LÓGICA DE NAVEGAÇÃO ==================

void MainComponent::showPadScreen(juce::String kitFile)
{
    menuScreen->setVisible(false);
    removeChildComponent(menuScreen.get());
    padGridScreen = std::make_unique<PadGridComponent>(mixerSource, kitFile);
    
    padGridScreen->backButton.addListener(this);
    addAndMakeVisible(*padGridScreen);
    padGridScreen->setBounds(getLocalBounds());
}

void MainComponent::showSettingsScreen()
{
    // 1. Esconde o menu
    menuScreen->setVisible(false);
    removeChildComponent(menuScreen.get());

    // 2. CRIA a tela de configurações, se ainda não existir
    if (settingsScreen == nullptr)
    {
        settingsScreen = std::make_unique<SettingsComponent>();
        // O MainComponent começa a escutar o botão "Voltar"
        settingsScreen->backButton.addListener(this); 
    }

    // 3. Adiciona e torna visível
    addAndMakeVisible(*settingsScreen);
    settingsScreen->setVisible(true);
    settingsScreen->setBounds(getLocalBounds());
}

void MainComponent::showMenuScreen()
{
    // Esconde qualquer outra tela que estiver ativa (Mixer ou Pads)
    if (settingsScreen != nullptr)
    {
        settingsScreen->setVisible(false);
        removeChildComponent(settingsScreen.get());
    }
    
    if (padGridScreen != nullptr)
    {
        padGridScreen->backButton.removeListener(this);
        padGridScreen->setVisible(false);
        removeChildComponent(padGridScreen.get());
        padGridScreen.reset();
    }

    // Traz o menu de volta à vida
    addAndMakeVisible(*menuScreen);
    menuScreen->setVisible(true);
    menuScreen->setBounds(getLocalBounds());
}

void MainComponent::buttonClicked(juce::Button* button)
{
    // Tela de Menu
    if (menuScreen != nullptr)
    {
        if (button == &menuScreen->kitMelodiaButton)
            showPadScreen("notes_json");
        else if (button == &menuScreen->kitBateriaButton)
            showPadScreen("drumkit_json");
        else if (button == &menuScreen->settingsButton)
            showSettingsScreen(); // Vai para as Configurações
    }

    // Tela de Pads
    if (padGridScreen != nullptr)
    {
        if (button == &padGridScreen->backButton)
        {
            showMenuScreen(); // Volta para o menu principal
        }
    }
    
    // Tela de Configurações/Mixer
    if (settingsScreen != nullptr)
    {
        if (button == &settingsScreen->backButton)
            showMenuScreen(); // Volta para o Menu
    }
}