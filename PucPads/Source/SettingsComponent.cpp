#include "SettingsComponent.h"

SettingsComponent::SettingsComponent()
{
    // Configuração visual dos Sliders (Dourado Neon)
    auto goldNeon = juce::Colour::fromString("ffb08d35");

    // Slider Master
    masterVolumeSlider.setSliderStyle(juce::Slider::LinearVertical);
    masterVolumeSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    masterVolumeSlider.setColour(juce::Slider::thumbColourId, juce::Colours::white);
    masterVolumeSlider.setColour(juce::Slider::trackColourId, goldNeon);
    addAndMakeVisible(masterVolumeSlider);
    addAndMakeVisible(masterLabel);
    masterLabel.setJustificationType(juce::Justification::centred);

    // Slider Backing Track
    backingTrackSlider.setSliderStyle(juce::Slider::LinearVertical);
    backingTrackSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    backingTrackSlider.setColour(juce::Slider::thumbColourId, juce::Colours::white);
    backingTrackSlider.setColour(juce::Slider::trackColourId, goldNeon);
    addAndMakeVisible(backingTrackSlider);
    addAndMakeVisible(backingLabel);
    backingLabel.setJustificationType(juce::Justification::centred);

    // Botão Voltar
    backButton.setColour(juce::TextButton::buttonColourId, juce::Colour::fromString("ff420012")); // Bordô
    addAndMakeVisible(backButton);
}

SettingsComponent::~SettingsComponent() {}

void SettingsComponent::paint(juce::Graphics& g)
{
    // Mesmo Bordô profundo do fundo da grade
    g.fillAll(juce::Colour::fromString("ff2a000b"));
    
    g.setColour(juce::Colours::white);
    g.setFont(24.0f);
    g.drawText("MIXER & CONFIGURACOES", getLocalBounds().removeFromTop(50), juce::Justification::centred);
}

void SettingsComponent::resized()
{
    auto bounds = getLocalBounds();
    backButton.setBounds(bounds.removeFromBottom(60).reduced(20, 10));

    auto areaMixer = bounds.reduced(50);
    int sliderWidth = areaMixer.getWidth() / 2;

    auto area1 = areaMixer.removeFromLeft(sliderWidth);
    masterLabel.setBounds(area1.removeFromBottom(30));
    masterVolumeSlider.setBounds(area1.reduced(20));

    auto area2 = areaMixer;
    backingLabel.setBounds(area2.removeFromBottom(30));
    backingTrackSlider.setBounds(area2.reduced(20));
}