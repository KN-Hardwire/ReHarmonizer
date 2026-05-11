#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <cmath>

juce::AudioProcessorValueTreeState::ParameterLayout ReHarmonizerAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(paramBlend, 1),
        "Blend",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f),
        0.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(paramPitchCorrect, 1),
        "Pitch",
        juce::NormalisableRange<float>(-12.0f, 12.0f, 0.1f),
        0.0f,
        "st"));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(paramGainDb, 1),
        "Gain",
        juce::NormalisableRange<float> (-60.0f, 3.0f, 0.1f),
        0.0f,
        "dB"));

    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID(paramWaveform, 1),
        "Waveform",
        juce::StringArray{ "Sine", "Square", "Sawtooth", "Triangle" },
        0));

    return layout;
}

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
    return juce::String();
}

void ReHarmonizerAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
    juce::ignoreUnused(index, newName);
}

//==============================================================================
void ReHarmonizerAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::ignoreUnused(samplesPerBlock);
    freqDetector.prepare(sampleRate);
    oscillator.setSampleRate(sampleRate);
    oscillator.reset();
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
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();

    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        float monoSample = 0.0f;
        for (int channel = 0; channel < totalNumInputChannels; ++channel) {
            monoSample += buffer.getReadPointer(channel)[sample];
        }   // Summing samples from all channels
        monoSample /= static_cast<float>(totalNumInputChannels); // Calculate average to remove distortion
        freqDetector.processSample(monoSample);
        dominantFrequency.store(freqDetector.getFrequency());
    }

    const auto detectedHz = dominantFrequency.load();
    const bool detectedHzValid = std::isfinite(detectedHz) && detectedHz >= 20.0f && detectedHz <= 20000.0f;
    if (!detectedHzValid)
        return;

    const float blend = juce::jlimit(0.0f, 1.0f, apvts.getRawParameterValue(paramBlend)->load());
    const float pitchCorrectSemis = apvts.getRawParameterValue(paramPitchCorrect)->load();
    const float gainDb = apvts.getRawParameterValue(paramGainDb)->load();
    const int waveformIndex = static_cast<int>(apvts.getRawParameterValue(paramWaveform)->load());

    oscillator.setWaveform(static_cast<Oscillator::Waveform>(juce::jlimit (0, 3, waveformIndex)));

    const float pitchRatio = std::pow(2.0f, pitchCorrectSemis / 12.0f);
    oscillator.setFrequency(detectedHz * pitchRatio);

    const float oscGain = juce::Decibels::decibelsToGain(gainDb);
    const float dryMix = 1.0f - blend;

    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();

    for (int sample = 0; sample < numSamples; ++sample)
    {
        const float oscSample = oscillator.processSample() * oscGain;

        for (int channel = 0; channel < numChannels; ++channel)
        {
            auto* writePtr = buffer.getWritePointer(channel);
            writePtr[sample] = writePtr[sample] * dryMix + oscSample * blend;
        }
    }
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
    auto state = apvts.copyState();
    if (auto xml = state.createXml())
        copyXmlToBinary(*xml, destData);
}

void ReHarmonizerAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    if (auto xmlState = getXmlFromBinary(data, sizeInBytes))
        apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

//==============================================================================
// Creates new instances of the plugin
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ReHarmonizerAudioProcessor();
}
