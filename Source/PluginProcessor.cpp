#include "PluginProcessor.h"
#include "PluginEditor.h"

ReHarmonizerAudioProcessor::ReHarmonizerAudioProcessor()
{
    
}

ReHarmonizerAudioProcessor::~ReHarmonizerAudioProcessor()
{
    
}

//==============================================================================
const juce::String ReHarmonizerAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ReHarmonizerAudioProcessor::acceptsMidi() const
{
    return false;
}

bool ReHarmonizerAudioProcessor::producesMidi() const
{
    return false;
}

bool ReHarmonizerAudioProcessor::isMidiEffect() const
{
    return false;
}

double ReHarmonizerAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ReHarmonizerAudioProcessor::getNumPrograms()
{
    return 1;
}

int ReHarmonizerAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ReHarmonizerAudioProcessor::setCurrentProgram(int index)
{
    juce::ignoreUnused(index);
}

const juce::String ReHarmonizerAudioProcessor::getProgramName(int index)
{
    juce::ignoreUnused(index);
    return juce::String();  // return empty string
}

void ReHarmonizerAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
    juce::ignoreUnused(index, newName);
}

//==============================================================================
void ReHarmonizerAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::ignoreUnused(sampleRate, samplesPerBlock);
}

void ReHarmonizerAudioProcessor::releaseResources()
{
    
}

bool ReHarmonizerAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    // Reject layout with no input - need at least a mono input
    if (layouts.getMainInputChannelSet() == juce::AudioChannelSet::disabled())
        return false;
    // Output layout must match input layout
    if (layouts.getMainInputChannelSet() != layouts.getMainOutputChannelSet())
        return false;
    
    return true;
}

void ReHarmonizerAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused(midiMessages);
    
//    auto totalNumInputChannels{ getTotalNumInputChannels() };
//    auto totalNumOutputChannels{ getTotalNumOutputChannels() };
}

//==============================================================================
bool ReHarmonizerAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* ReHarmonizerAudioProcessor::createEditor()
{
    return new ReHarmonizerAudioProcessorEditor(*this);
}

//==============================================================================
void ReHarmonizerAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    juce::ignoreUnused(destData);
}

void ReHarmonizerAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    juce::ignoreUnused(data, sizeInBytes);
}

//==============================================================================
// Creates new instances of the plugin
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ReHarmonizerAudioProcessor();
}
