#pragma once

#include <JuceHeader.h>
#include "PadComponent.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  : public juce::AudioAppComponent
{
public:
    
    MainComponent();
    ~MainComponent() override;

    // Funções de áudio
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;
    
    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
	juce::MixerAudioSource mixerSource;
	juce::OwnedArray<PadComponent> pads;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
