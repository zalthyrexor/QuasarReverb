#include "PluginProcessor.h"
#include "PluginEditor.h"



QuasarReverbAudioProcessorEditor::QuasarReverbAudioProcessorEditor(QuasarReverbAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    mixSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    mixSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(mixSlider);

    mixAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "MIX", mixSlider);

    rt60Slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    addAndMakeVisible(rt60Slider);
    rt60Attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "RT60", rt60Slider);

    setSize(400, 300);
}
QuasarReverbAudioProcessorEditor::~QuasarReverbAudioProcessorEditor()
{
}
void QuasarReverbAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced(20);
    mixSlider.setBounds(area.removeFromLeft(150).reduced(10));
    rt60Slider.setBounds(area.removeFromLeft(150).reduced(10));
}

void QuasarReverbAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    g.setColour(juce::Colours::white);
    g.drawText("Mix", mixSlider.getBounds().withY(mixSlider.getBottom()), juce::Justification::centred, true);
    g.drawText("Decay", rt60Slider.getBounds().withY(rt60Slider.getBottom()), juce::Justification::centred, true);
}
