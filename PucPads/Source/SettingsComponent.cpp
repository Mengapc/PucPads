#include "SettingsComponent.h"
#include "UiTheme.h"

//==============================================================================
SettingsComponent::SettingsComponent()
{
    configureSlider (masterVolumeSlider, masterLabel);
    configureSlider (backingTrackSlider, backingLabel);

    // O MainComponent sobrescreve com o valor real ao abrir a tela; 100% aqui
    // evita que a primeira abertura apareca com o slider em zero.
    setMasterVolume (1.0f);
    setBackingVolume (1.0f);

    masterVolumeSlider.onValueChange = [this]
    {
        if (onMasterVolumeChanged != nullptr)
            onMasterVolumeChanged (static_cast<float> (masterVolumeSlider.getValue()));
    };

    backingTrackSlider.onValueChange = [this]
    {
        if (onBackingVolumeChanged != nullptr)
            onBackingVolumeChanged (static_cast<float> (backingTrackSlider.getValue()));
    };

    // Botão Voltar
    UiTheme::applyButtonStyle (backButton);
    addAndMakeVisible (backButton);
}

SettingsComponent::~SettingsComponent() {}

//==============================================================================
void SettingsComponent::configureSlider (juce::Slider& slider, juce::Label& label)
{
    slider.setSliderStyle (juce::Slider::LinearVertical);
    slider.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);

    // Ganho linear 0..1 com curva: o meio do curso fica em ~-12 dB, onde o
    // ajuste fino importa mais do que no topo da escala.
    slider.setRange (0.0, 1.0, 0.0);
    slider.setSkewFactorFromMidPoint (0.25);

    slider.setColour (juce::Slider::thumbColourId, juce::Colours::white);
    slider.setColour (juce::Slider::trackColourId, UiTheme::bordaDourada());

    addAndMakeVisible (slider);

    label.setJustificationType (juce::Justification::centred);
    label.setColour (juce::Label::textColourId, UiTheme::dourado());
    addAndMakeVisible (label);
}

//==============================================================================
void SettingsComponent::setMasterVolume (float linearGain)
{
    masterVolumeSlider.setValue (juce::jlimit (0.0, 1.0, (double) linearGain),
                                 juce::dontSendNotification);
}

void SettingsComponent::setBackingVolume (float linearGain)
{
    backingTrackSlider.setValue (juce::jlimit (0.0, 1.0, (double) linearGain),
                                 juce::dontSendNotification);
}

//==============================================================================
void SettingsComponent::paint (juce::Graphics& g)
{
    // Mesmo Bordô profundo do fundo da grade
    g.fillAll (UiTheme::bordoEscuro());

    g.setColour (UiTheme::dourado());
    g.setFont (24.0f);
    g.drawText ("MIXER & CONFIGURACOES", getLocalBounds().removeFromTop (50),
                juce::Justification::centred);
}

void SettingsComponent::resized()
{
    auto bounds = getLocalBounds();
    backButton.setBounds (bounds.removeFromBottom (60).reduced (20, 10));

    auto areaMixer = bounds.reduced (50);
    const int sliderWidth = areaMixer.getWidth() / 2;

    auto area1 = areaMixer.removeFromLeft (sliderWidth);
    masterLabel.setBounds (area1.removeFromBottom (30));
    masterVolumeSlider.setBounds (area1.reduced (20));

    auto area2 = areaMixer;
    backingLabel.setBounds (area2.removeFromBottom (30));
    backingTrackSlider.setBounds (area2.reduced (20));
}
