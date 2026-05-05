#include "PluginProcessor.h"
#include "PluginEditor.h"

ReHarmonizerAudioProcessor::ReHarmonizerAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                   )
#endif
{}

ReHarmonizerAudioProcessor::~ReHarmonizerAudioProcessor() {}

const juce::String ReHarmonizerAudioProcessor::getName() const { return JucePlugin_Name; }
bool ReHarmonizerAudioProcessor::acceptsMidi() const { return false; }
bool ReHarmonizerAudioProcessor::producesMidi() const { return false; }
bool ReHarmonizerAudioProcessor::isMidiEffect() const { return false; }
double ReHarmonizerAudioProcessor::getTailLengthSeconds() const { return 0.0; }
int ReHarmonizerAudioProcessor::getNumPrograms() { return 1; }
int ReHarmonizerAudioProcessor::getCurrentProgram() { return 0; }
void ReHarmonizerAudioProcessor::setCurrentProgram (int index) {}
const juce::String ReHarmonizerAudioProcessor::getProgramName (int index) { return {}; }
void ReHarmonizerAudioProcessor::changeProgramName (int index, const juce::String& newName) {}

void ReHarmonizerAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock) {}
void ReHarmonizerAudioProcessor::releaseResources() {}

bool ReHarmonizerAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    return layouts.getMainOutputChannelSet() == layouts.getMainInputChannelSet();
}

void ReHarmonizerAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
}

bool ReHarmonizerAudioProcessor::hasEditor() const { return true; }

juce::AudioProcessorEditor* ReHarmonizerAudioProcessor::createEditor()
{
    return new ReHarmonizerAudioProcessorEditor (*this);
}

void ReHarmonizerAudioProcessor::getStateInformation (juce::MemoryBlock& destData) {}
void ReHarmonizerAudioProcessor::setStateInformation (const void* data, int sizeInBytes) {}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ReHarmonizerAudioProcessor();
}