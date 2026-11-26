#pragma once

#include <JuceHeader.h>
#include "KitDefinition.h"

//==============================================================================
/*
    Esta classe representa um unico pad na nossa interface.
*/
class PadComponent : public juce::Component,
                     public juce::Timer
{
public:
    PadComponent(const KitPad& padSpec, const char* soundData, int soundDataSize,
                 juce::MixerAudioSource& mixerToUse);

    // Mantido por compatibilidade: cria um pad sem metadados (apenas a cor).
    PadComponent(const char* soundData, int soundDataSize,
                 juce::MixerAudioSource& mixerToUse, juce::Colour padColour);

    ~PadComponent() override;

    juce::AudioTransportSource& getTransportSource() { return transportSource; }

    int getChokeGroup() const noexcept { return chokeGroup; }
    juce::String getNoteName() const { return noteName; }

    // Pad fantasma (templates sem kit): desenhado, mas mudo e nao interativo.
    bool isEmptyPad() const noexcept { return ! hasAudio; }

    // (Des)liga o pad via choke group / troca de pagina, com fade curto para evitar estalos.
    void chokeStop();

    // Disparado quando este pad e acionado. O PadGridComponent usa para resolver
    // os grupos de exclusao mutua.
    std::function<void (PadComponent*)> onTriggered;

    void paint (juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

    // Funcoes de evento do mouse
    void mouseDown (const juce::MouseEvent& event) override;
    void mouseUp (const juce::MouseEvent& event) override;
    void mouseExit(const juce::MouseEvent& event) override;

private:
    void setupAudio(const char* soundData, int soundDataSize, juce::MixerAudioSource& mixerToUse);
    void finishChokeStop();

    // Selo com o numero do choke group (canto superior direito). Grupos valem
    // por template: o pad mostra a que grupo pertence para o usuario entender
    // por que um pad corta o outro.
    void paintChokeBadge (juce::Graphics& g, juce::Colour groupColour) const;

    static constexpr int chokeFadeMs = 25; // duracao do fade de choke (15-30 ms)

    juce::MixerAudioSource& mixerRef;

    // Variavel para guardar estado do mouse
    bool isMouseDown = false;

    // Variavel para controle de animacao do brilho
    float currentFade = 0.0f;

    // Metadados vindos do kit
    juce::Colour activeColour;
    juce::String noteName;
    int chokeGroup = 0;
    bool oneShot = false;
    bool hasAudio = false; // false = pad fantasma (sem sample carregado)
    float baseGain = 1.0f;

    // Invalida um chokeStop pendente quando o pad e reacionado dentro da janela de fade
    int chokeToken = 0;

    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transportSource;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PadComponent)
};
