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

    // Rampa de 50 ms: tempo suficiente para o slider nao estalar e curto o
    // bastante para a resposta parecer imediata.
    masterGainRamp.reset(sampleRate, 0.05);
    masterGainRamp.setCurrentAndTargetValue(masterGain.load());
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    // O mixer pode delegar a uma unica fonte que nao preenche todos os canais;
    // limpar o bloco garante silencio em vez de repetir lixo do buffer anterior.
    bufferToFill.clearActiveBufferRegion();

    mixerSource.getNextAudioBlock(bufferToFill);

    if (bufferToFill.buffer == nullptr || bufferToFill.numSamples <= 0)
        return;

    // Rampa por bloco (independente de canal) a partir do valor atual: nao
    // depende de startSample, ao contrario de SmoothedValue::applyGain(buffer).
    masterGainRamp.setTargetValue(masterGain.load());

    const auto gainInicio = masterGainRamp.getCurrentValue();
    const auto gainFim    = masterGainRamp.skip(bufferToFill.numSamples);

    bufferToFill.buffer->applyGainRamp(bufferToFill.startSample, bufferToFill.numSamples,
                                       gainInicio, gainFim);
}

void MainComponent::releaseResources()
{
    mixerSource.releaseResources();
}

void MainComponent::setMasterGain(float linearGain)
{
    // Só o valor é publicado aqui; o audio thread aplica com rampa.
    masterGain.store(juce::jlimit(0.0f, 1.0f, linearGain));
}

void MainComponent::setBackingGain(float linearGain)
{
    backingGain = juce::jlimit(0.0f, 1.0f, linearGain);

    if (launchPadScreen != nullptr)
        launchPadScreen->setBackingGain(backingGain);
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
    launchPadScreen->setBackingGain(backingGain); // conserva o volume do mixer entre entradas

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

        // Os sliders só publicam o valor; quem aplica o ganho é este componente.
        settingsScreen->onMasterVolumeChanged = [this](float value) { setMasterGain(value); };
        settingsScreen->onBackingVolumeChanged = [this](float value) { setBackingGain(value); };
    }

    // 3. Sliders refletem o estado real do áudio ao abrir a tela
    settingsScreen->setMasterVolume(masterGain.load());
    settingsScreen->setBackingVolume(backingGain);

    // 4. Adiciona e torna visível
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
