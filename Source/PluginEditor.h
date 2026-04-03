#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class QuasarReverbAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    QuasarReverbAudioProcessorEditor (QuasarReverbAudioProcessor&);
    ~QuasarReverbAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    juce::Slider mixSlider;
    juce::Slider rt60Slider;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mixAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> rt60Attachment;

    QuasarReverbAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (QuasarReverbAudioProcessorEditor)
};
