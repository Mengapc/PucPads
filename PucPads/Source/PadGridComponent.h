#pragma once

#include <JuceHeader.h>
#include "PadComponent.h"
#include "KitDefinition.h"

//==============================================================================
/*
    Grade de performance de UM template (um arquivo de kit).

    O cromo da tela (voltar, play/stop da base e os 8 botoes de template) pertence
    ao LaunchPadComponent; aqui ficam apenas os pads da grade.

    Um template = uma grade. Variedade dentro do template vem dos choke groups
    (RF02), nao de paginas: todos os pads de um kit dividem o mesmo 8x8 e cada
    pad sinaliza a que grupo pertence (borda + selo numerico). O parser continua
    lendo kits multi-pagina (schema v2) apenas por retrocompatibilidade: neste
    caso so a primeira pagina e exibida (ver loadKit()).

    kitResource vazio => template ainda sem kit: o componente monta uma grade
    fantasma (sem audio), pronta para receber os pads pelo futuro editor.
*/
class PadGridComponent : public juce::Component
{
public:
    PadGridComponent (juce::MixerAudioSource& mixerToUse, const juce::String& kitResource);
    ~PadGridComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    //--- Base musical (comandada pela barra de controles do LaunchPadComponent) ---
    bool hasBackingTrack() const noexcept { return backingLoaded; }
    void playBackingTrack();
    void stopBackingTrack();

    // Interrompe todos os samples em execucao (troca de template ou saida da tela).
    void stopAllPads();

    bool isEmptyTemplate() const noexcept { return isGhostTemplate; }

    // Quantidade de pads instanciados; usado pelos testes de fumaca.
    int getNumPads() const noexcept { return pads.size(); }

private:
    void loadKit (const juce::String& kitResource);
    void loadBackingTrack (const juce::String& trackName);
    void buildPads (const KitPage& kitPage);
    void buildEmptyTemplate();
    void layoutPads();

    void handlePadTriggered (PadComponent* triggerer);

    juce::MixerAudioSource& mixerSource;

    KitDefinition kit;
    juce::OwnedArray<PadComponent> pads;
    bool isGhostTemplate = false;
    bool backingLoaded = false;

    juce::AudioFormatManager formatManager; // Gerenciador de formatos (mp3, wav)
    std::unique_ptr<juce::AudioFormatReaderSource> backgroundReaderSource;
    juce::AudioTransportSource backgroundSource; // O "Player" da base

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PadGridComponent)
};
