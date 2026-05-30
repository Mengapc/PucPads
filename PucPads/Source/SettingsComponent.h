#pragma once
#include <JuceHeader.h>

class SettingsComponent : public juce::Component
{
public:
    SettingsComponent();
    ~SettingsComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    // Botão para voltar para a tela de pads
    juce::TextButton backButton{ "VOLTAR" };

    // Sliders de Volume (estilo Mixer)
    juce::Slider masterVolumeSlider;
    juce::Slider backingTrackSlider;

    juce::Label masterLabel{ {}, "MASTER VOLUME" };
    juce::Label backingLabel{ {}, "BASE VOLUME" };

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SettingsComponent)
};