#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    menuScreen = std::make_unique<MenuComponent>();

    menuScreen->launchPadButton.addListener(this);
    menuScreen->settingsButton.addListener(this);

    addAndMakeVisible(*menuScreen);

    setAudioChannels(0, 2);
    setSize(450, 700);
}

MainComponent::~MainComponent()
{
    shutdownAudio();

    menuScreen->launchPadButton.removeListener(this);
    menuScreen->settingsButton.removeListener(this);

    if (settingsScreen != nullptr)
        settingsScreen->backButton.removeListener(this);

    if (launchPadScreen != nullptr)
        launchPadScreen->backButton.removeListener(this);
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

    if (launchPadScreen != nullptr && launchPadScreen->isVisible())
        launchPadScreen->setBounds(getLocalBounds());

    // Redimensiona o Mixer caso ele seja a tela ativa
    if (settingsScreen != nullptr && settingsScreen->isVisible())
        settingsScreen->setBounds(getLocalBounds());
}

//================== A LÓGICA DE NAVEGAÇÃO ==================

void MainComponent::showLaunchPadScreen()
{
    // So existe uma tela de performance: as antigas "Kit Melodia" e "Kit Bateria"
    // viraram templates dentro do LaunchPad.
    menuScreen->setVisible(false);
    removeChildComponent(menuScreen.get());

    // Recriada a cada entrada: garante audio limpo ao voltar do menu
    if (launchPadScreen != nullptr)
    {
        launchPadScreen->backButton.removeListener(this);
        launchPadScreen.reset();
    }

    launchPadScreen = std::make_unique<LaunchPadComponent>(mixerSource);
    launchPadScreen->backButton.addListener(this);

    addAndMakeVisible(*launchPadScreen);
    launchPadScreen->setBounds(getLocalBounds());
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
    // Esconde qualquer outra tela que estiver ativa (Mixer ou LaunchPad)
    if (settingsScreen != nullptr)
    {
        settingsScreen->setVisible(false);
        removeChildComponent(settingsScreen.get());
    }

    if (launchPadScreen != nullptr)
    {
        launchPadScreen->backButton.removeListener(this);
        launchPadScreen->setVisible(false);
        removeChildComponent(launchPadScreen.get());
        launchPadScreen.reset();
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
        if (button == &menuScreen->launchPadButton)
            showLaunchPadScreen();
        else if (button == &menuScreen->settingsButton)
            showSettingsScreen();
    }

    // Tela de LaunchPad
    if (launchPadScreen != nullptr)
    {
        if (button == &launchPadScreen->backButton)
            showMenuScreen(); // Volta para o menu principal
    }

    // Tela de Configurações/Mixer
    if (settingsScreen != nullptr)
    {
        if (button == &settingsScreen->backButton)
            showMenuScreen(); // Volta para o Menu
    }
}
