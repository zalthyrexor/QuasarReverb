#include "PluginProcessor.h"
#include "PluginEditor.h"

QuasarReverbAudioProcessor::QuasarReverbAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), apvts(*this, nullptr, "Parameters", createParameterLayout())
#endif
{
}
QuasarReverbAudioProcessor::~QuasarReverbAudioProcessor()
{
}
const juce::String QuasarReverbAudioProcessor::getName() const
{
    return JucePlugin_Name;
}
bool QuasarReverbAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}
bool QuasarReverbAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}
bool QuasarReverbAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}
double QuasarReverbAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}
int QuasarReverbAudioProcessor::getNumPrograms()
{
    return 1;
}
int QuasarReverbAudioProcessor::getCurrentProgram()
{
    return 0;
}
void QuasarReverbAudioProcessor::setCurrentProgram (int index)
{
}
const juce::String QuasarReverbAudioProcessor::getProgramName (int index)
{
    return {};
}
void QuasarReverbAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}
void QuasarReverbAudioProcessor::releaseResources()
{
}
#ifndef JucePlugin_PreferredChannelConfigurations
bool QuasarReverbAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif



void QuasarReverbAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    engine.prepare(static_cast<int>(sampleRate));
}
void QuasarReverbAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
    {
        buffer.clear(i, 0, buffer.getNumSamples());
    }
    engine.setMix(*apvts.getRawParameterValue("MIX"));
    engine.updateReverbTimes(*apvts.getRawParameterValue("RT60"));
    auto* leftData = buffer.getWritePointer(0);
    auto* rightData = buffer.getWritePointer(1);

    for (int i = 0; i < buffer.getNumSamples(); ++i)
    {
        engine.process(leftData[i], rightData[i]);
    }
}



bool QuasarReverbAudioProcessor::hasEditor() const
{
    return true;
}
juce::AudioProcessorEditor* QuasarReverbAudioProcessor::createEditor()
{
    return new QuasarReverbAudioProcessorEditor (*this);
}
void QuasarReverbAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
}
void QuasarReverbAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
}
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new QuasarReverbAudioProcessor();
}
