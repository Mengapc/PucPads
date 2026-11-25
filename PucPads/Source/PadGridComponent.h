#pragma once

#include <JuceHeader.h>
#include "PadComponent.h"
#include "KitDefinition.h"

//==============================================================================
class PadGridComponent : public juce::Component
{
public:
    // Nosso construtor vai receber o mixer principal
    PadGridComponent(juce::MixerAudioSource& mixerToUse, juce::String kitFileToLoad);
    ~PadGridComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;

    juce::TextButton backButton{ " < " };

private:
    // Um banco de pads exibido por vez. Todos os pads sao criados no load (audio
    // carregado uma unica vez); a troca de pagina so alterna visibilidade/layout.
    struct Page
    {
        juce::String name;
        juce::OwnedArray<PadComponent> pads;
    };

    void loadBackingTrack(const juce::String& trackName);
    void buildPages();
    void buildPageButtons();

    void showPage(int pageIndex);
    void updatePageButtons();
    void layoutPage();
    void layoutPageButtons(juce::Rectangle<int> area);

    void handlePadTriggered(PadComponent* triggerer);

    juce::MixerAudioSource& mixerSource;

    KitDefinition kit;
    juce::OwnedArray<Page> pageViews;
    juce::OwnedArray<juce::TextButton> pageButtons;
    int currentPage = 0;

    juce::AudioFormatManager formatManager; // Gerenciador de formatos (mp3, wav)
    std::unique_ptr<juce::AudioFormatReaderSource> backgroundReaderSource;
    juce::AudioTransportSource backgroundSource; // O "Player" da base

    // Botoes de controle
    juce::TextButton playButton{ "Play Base" };
    juce::TextButton stopButton{ "Stop Base" };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PadGridComponent)
};
