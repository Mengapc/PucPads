#pragma once
#include <JuceHeader.h>

//==============================================================================
/*
    Tela de Mixer & Configuracoes.

    Os sliders nao aplicam ganho diretamente: eles avisam o MainComponent (dono
    do MixerAudioSource e da cadeia de audio), que guarda o valor e o aplica no
    audio thread. Assim a tela continua sendo so interface.
*/
class SettingsComponent : public juce::Component
{
public:
    SettingsComponent();
    ~SettingsComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    // Ganho linear (0..1). Disparados quando o usuario move os sliders.
    std::function<void (float)> onMasterVolumeChanged;
    std::function<void (float)> onBackingVolumeChanged;

    // Posiciona o slider conforme o estado real do audio, sem disparar os
    // callbacks acima (evita realimentacao entre UI e mixer).
    void setMasterVolume (float linearGain);
    void setBackingVolume (float linearGain);

    // Botão para voltar para a tela de pads
    juce::TextButton backButton{ "VOLTAR" };

    // Sliders de Volume (estilo Mixer)
    juce::Slider masterVolumeSlider;
    juce::Slider backingTrackSlider;

    juce::Label masterLabel{ {}, "MASTER VOLUME" };
    juce::Label backingLabel{ {}, "BASE VOLUME" };

private:
    void configureSlider (juce::Slider& slider, juce::Label& label);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SettingsComponent)
};
