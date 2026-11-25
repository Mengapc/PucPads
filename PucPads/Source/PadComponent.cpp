#include <JuceHeader.h>
#include "PadComponent.h"
#include "UiTheme.h"

//==============================================================================
namespace
{
    KitPad makeColourOnlySpec (juce::Colour colour)
    {
        KitPad spec;
        spec.colour = colour;
        return spec;
    }
}

//==============================================================================
PadComponent::PadComponent(const KitPad& padSpec, const char* soundData, int soundDataSize,
                           juce::MixerAudioSource& mixerToUse)
    : mixerRef(mixerToUse)
{
    activeColour = padSpec.colour;
    noteName     = padSpec.note;
    chokeGroup   = padSpec.chokeGroup;
    oneShot      = padSpec.oneShot;
    baseGain     = juce::Decibels::decibelsToGain (padSpec.gainDb);

    setupAudio(soundData, soundDataSize, mixerToUse);
}

PadComponent::PadComponent(const char* soundData, int soundDataSize,
                           juce::MixerAudioSource& mixerToUse, juce::Colour padColour)
    : PadComponent(makeColourOnlySpec(padColour), soundData, soundDataSize, mixerToUse)
{
}

PadComponent::~PadComponent()
{
    stopTimer();

    mixerRef.removeInputSource(&transportSource);
}

void PadComponent::paint (juce::Graphics& g)
{
    // Pad fantasma (template ainda sem kit): so a silhueta da grade, sem cor de funcao.
    if (! hasAudio)
    {
        g.fillAll (UiTheme::bordo().darker (0.35f));
        g.setColour (UiTheme::bordaDourada().withAlpha (0.25f));
        g.drawRoundedRectangle (getLocalBounds().reduced (1).toFloat(), 4.0f, 1.0f);
        return;
    }

    juce::Colour corFundoInativo = UiTheme::bordo();
    juce::Colour corBordaInativa = UiTheme::bordaDourada();

    // A MAGICA DA COR: Mistura o fundo inativo com a cor Neon baseada no "currentFade"
    juce::Colour corAtual = corFundoInativo.interpolatedWith(activeColour, currentFade);

    g.fillAll(corAtual);

    // Se o brilho estiver alto (> 0.8), a borda fica branca (estouro de luz)
    // Se estiver diminuindo, a borda volta a ser dourada
    g.setColour(currentFade > 0.8f ? juce::Colours::white : corBordaInativa);

    g.drawRoundedRectangle(getLocalBounds().reduced(1).toFloat(), 4.0f, 1.5f);

    // Rotulo do pad (ex.: "C", "D"), util na pagina de melodia. Fica oculto em pads minusculos.
    if (noteName.isNotEmpty() && getWidth() > 34 && getHeight() > 22)
    {
        g.setColour(juce::Colours::white.withAlpha(0.85f));
        g.setFont(juce::Font(juce::FontOptions(11.0f)));
        g.drawText(noteName, getLocalBounds().reduced(4, 2),
                   juce::Justification::centredBottom, false);
    }
}

void PadComponent::resized()
{
}

void PadComponent::mouseDown (const juce::MouseEvent& event)
{
    // Pad fantasma nao reage ao toque: sinalizar brilho daria a falsa impressao de som
    if (! hasAudio)
        return;

    isMouseDown = true;

    // Forca o brilho para o maximo instantaneamente
    currentFade = 1.0f;
    stopTimer();

    // Reativa um choke pendente: o stop agendado nao deve atingir este novo disparo
    ++chokeToken;

    // Resolve os choke groups ANTES de tocar, para nao cortar o proprio pad
    if (onTriggered != nullptr)
        onTriggered(this);

    transportSource.setPosition(0);   // Rebobina o som para o inicio
    transportSource.setGain(baseGain);
    transportSource.start();          // Toca o som

    if (oneShot)
        startTimerHz(60);             // acompanha o fim do sample para apagar o brilho

    repaint();
}

void PadComponent::mouseUp (const juce::MouseEvent& event)
{
    if (! hasAudio)
        return;

    isMouseDown = false;

    if (! oneShot)
    {
        transportSource.stop();
        transportSource.setGain(baseGain);
    }

    // Inicia a animacao a 60 quadros por segundo (60 Hz)
    startTimerHz(60);
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

void PadComponent::chokeStop()
{
    const int token = ++chokeToken;

    if (! transportSource.isPlaying())
    {
        currentFade = 0.0f;
        repaint();
        return;
    }

    // O AudioTransportSource aplica setGain com rampa interna, o que evita o clique
    // de um corte seco; o stop efetivo vem um instante depois.
    transportSource.setGain(0.0f);

    juce::Component::SafePointer<PadComponent> safeThis(this);
    juce::Timer::callAfterDelay(chokeFadeMs, [safeThis, token]() mutable
    {
        if (auto* pad = safeThis.getComponent())
            if (pad->chokeToken == token)
                pad->finishChokeStop();
    });
}

void PadComponent::finishChokeStop()
{
    transportSource.stop();
    transportSource.setGain(baseGain);

    stopTimer();
    currentFade = 0.0f;
    repaint();
}

void PadComponent::setupAudio(const char* soundData, int soundDataSize, juce::MixerAudioSource& mixerToUse)
{
    // 1. Registrar os formatos de audio
    formatManager.registerBasicFormats();

    // 2. Template sem kit: pad fantasma, nada para carregar nem para mixar
    if (soundData == nullptr || soundDataSize <= 0)
        return;

    // 3. Criar um "leitor" para o som a partir da memoria
    auto* reader = formatManager.createReaderFor(std::make_unique<juce::MemoryInputStream>(soundData, soundDataSize, false));

    if (reader != nullptr)
    {
        // 4. Conectar o leitor ao "toca-fitas"
        std::unique_ptr<juce::AudioFormatReaderSource> newSource (new juce::AudioFormatReaderSource (reader, true));
        transportSource.setSource (newSource.get(), 0, nullptr, reader->sampleRate);
        readerSource.reset (newSource.release());
        transportSource.setGain(baseGain);
        mixerToUse.addInputSource(&transportSource, false);
        hasAudio = true;
    }
    else
    {
        DBG("ERRO: Falha ao carregar o arquivo de audio!");
    }
}

void PadComponent::timerCallback()
{
    // Modo one-shot: o brilho acompanha a duracao real do sample antes de apagar
    if (oneShot && transportSource.isPlaying() && ! transportSource.hasStreamFinished())
    {
        currentFade = 1.0f;
        repaint();
        return;
    }

    // Reduz o brilho (0.05 por frame = fade rapido. Mude para 0.02 para fade lento)
    currentFade -= 0.05f;

    if (currentFade <= 0.0f)
    {
        currentFade = 0.0f; // Trava no zero
        stopTimer();        // Desliga o timer para economizar CPU
    }

    repaint(); // Pede para desenhar a nova cor intermediaria
}
