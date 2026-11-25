#pragma once

#include <JuceHeader.h>
#include "PadComponent.h"
#include "KitDefinition.h"

//==============================================================================
/*
    Grade de performance de UM template (um arquivo de kit).

    O cromo da tela (voltar, play/stop da base e os 8 botoes de template) pertence
    ao LaunchPadComponent; aqui ficam apenas as paginas do kit e os pads.

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

    // Interrompe todos os samples em execucao (troca de pagina/template ou saida da tela).
    void stopAllPads();

    bool isEmptyTemplate() const noexcept { return isGhostTemplate; }

private:
    // Uma pagina (banco) de pads. Todos os pads sao criados no load (audio
    // carregado uma unica vez); a troca de pagina so alterna visibilidade/layout.
    struct Page
    {
        juce::String name;
        juce::OwnedArray<PadComponent> pads;
    };

    void loadBackingTrack (const juce::String& trackName);
    void buildPages();
    void buildEmptyTemplate();
    void buildPageButtons();

    void showPage (int pageIndex);
    void updatePageButtons();
    void layoutPage();
    void layoutPageButtons (juce::Rectangle<int> area);

    void handlePadTriggered (PadComponent* triggerer);

    // Faixa fina (so existe quando o kit tem mais de uma pagina) usada pelos
    // seletores de pagina. O rodape de 70 px agora e dos botoes de template.
    static constexpr int pageStripHeight = 34;

    juce::MixerAudioSource& mixerSource;

    KitDefinition kit;
    juce::OwnedArray<Page> pageViews;
    juce::OwnedArray<juce::TextButton> pageButtons;
    int currentPage = 0;
    bool isGhostTemplate = false;
    bool backingLoaded = false;

    juce::AudioFormatManager formatManager; // Gerenciador de formatos (mp3, wav)
    std::unique_ptr<juce::AudioFormatReaderSource> backgroundReaderSource;
    juce::AudioTransportSource backgroundSource; // O "Player" da base

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PadGridComponent)
};
