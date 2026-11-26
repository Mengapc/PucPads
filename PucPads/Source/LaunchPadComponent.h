#pragma once

#include <JuceHeader.h>
#include "PadGridComponent.h"

//==============================================================================
/*
    Tela unica de performance do app (substitui as antigas telas "Kit Melodia" e
    "Kit Bateria", que existiam so para os testes iniciais).

    Estrutura:
        cabecalho (60 px)  -> voltar + play/stop da base musical
        conteudo           -> PadGridComponent do template ativo
        rodape (70 px)     -> 8 botoes de template

    Cada botao do rodape e um template diferente de LaunchPad. Hoje apenas o
    template 1 tem kit (notes.json); os outros 7 existem sem JSON e abrem uma
    grade fantasma, que sera ligada ao editor de pads mais adiante.
*/
class LaunchPadComponent : public juce::Component
{
public:
    LaunchPadComponent (juce::MixerAudioSource& mixerToUse);
    ~LaunchPadComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    // Botao de retorno ao menu (o MainComponent escuta este botao)
    juce::TextButton backButton{ " < " };

    // Volume da base musical (0..1) de todos os templates. O valor fica guardado
    // aqui para valer tambem nos templates criados sob demanda depois.
    void setBackingGain (float gain);
    float getBackingGain() const noexcept { return backingGain; }

private:
    // Um slot do rodape. kitResource vazio = template ainda sem arquivo JSON.
    struct TemplateSlot
    {
        juce::String label;
        juce::String kitResource;
        std::unique_ptr<PadGridComponent> view;
    };

    static constexpr int numTemplates = 8;
    static constexpr int headerHeight  = 60;
    static constexpr int footerHeight  = 70;
    static constexpr int templateGap   = 6;

    void buildTemplateButtons();
    void showTemplate (int index);
    void updateTemplateButtons();
    void updateTransportButtons();
    void layoutTemplateButtons (juce::Rectangle<int> area);

    PadGridComponent* getActiveView() const noexcept;

    juce::MixerAudioSource& mixerSource;
    std::vector<TemplateSlot> templates;
    int currentTemplate = 0;
    float backingGain = 1.0f;

    juce::OwnedArray<juce::TextButton> templateButtons;

    // Controles da base musical (comandam o PadGridComponent ativo)
    juce::TextButton playButton{ "Play Base" };
    juce::TextButton stopButton{ "Stop Base" };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LaunchPadComponent)
};
