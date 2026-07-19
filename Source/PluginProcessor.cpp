#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <cmath>

namespace
{
    float computeEnvelopeStep(float timeMs, double sampleRate)
    {
        if (timeMs <= 0.0f)
            return 1.0f;

        return 1.0f / static_cast<float>(timeMs * 0.001 * sampleRate);
    }

    bool isPitchValid(float frequencyHz)
    {
        return std::isfinite(frequencyHz) && frequencyHz >= 20.0f && frequencyHz <= 20000.0f;
    }
}

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
        juce::NormalisableRange<float>(-60.0f, 3.0f, 0.1f),
        0.0f,
        "dB"));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(paramAttack, 1),
        "Attack",
        juce::NormalisableRange<float>(0.0f, 5000.0f, 1.0f),
        0.0f,
        "ms"));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(paramRelease, 1),
        "Release",
        juce::NormalisableRange<float>(0.0f, 5000.0f, 1.0f),
        0.0f,
        "ms"));

    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID(paramWaveform, 1),
        "Waveform",
        juce::StringArray{ "Sine", "Square", "Sawtooth", "Triangle" },
        0));

    return layout;
}

ReHarmonizerAudioProcessor::ReHarmonizerAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
	: AudioProcessor(BusesProperties()
		.withInput("Input", juce::AudioChannelSet::stereo(), true)
		.withOutput("Output", juce::AudioChannelSet::stereo(), true))
#endif
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
    return static_cast<double>(apvts.getRawParameterValue(paramRelease)->load()) * 0.001;
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
    currentSampleRate = sampleRate;
    freqDetector.prepare(sampleRate);
    oscillator.setSampleRate(sampleRate);
    oscillator.reset();
    envelopeLevel = 0.0f;
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

    const auto totalNumInputChannels = getTotalNumInputChannels();
    const auto numChannels = buffer.getNumChannels();
    const auto numSamples = buffer.getNumSamples();

    if (totalNumInputChannels == 0 || numSamples == 0)
        return;

    const float blend = juce::jlimit(0.0f, 1.0f, apvts.getRawParameterValue(paramBlend)->load());
    const float pitchCorrectSemis = apvts.getRawParameterValue(paramPitchCorrect)->load();
    const float gainDb = apvts.getRawParameterValue(paramGainDb)->load();
    const float attackMs = apvts.getRawParameterValue(paramAttack)->load();
    const float releaseMs = apvts.getRawParameterValue(paramRelease)->load();
    const int waveformIndex = static_cast<int>(apvts.getRawParameterValue(paramWaveform)->load());

    oscillator.setWaveform(static_cast<Oscillator::Waveform>(juce::jlimit(0, 3, waveformIndex)));

    const float pitchRatio = std::pow(2.0f, pitchCorrectSemis / 12.0f);
    const float oscGain = juce::Decibels::decibelsToGain(gainDb);
    const float dryMix = 1.0f - blend;
    const float attackStep = computeEnvelopeStep(attackMs, currentSampleRate);
    const float releaseStep = computeEnvelopeStep(releaseMs, currentSampleRate);

    for (int sample = 0; sample < numSamples; ++sample)
    {
        float monoSample = 0.0f;
        for (int channel = 0; channel < totalNumInputChannels; ++channel)
            monoSample += buffer.getReadPointer(channel)[sample];

        monoSample /= static_cast<float>(totalNumInputChannels);

        freqDetector.processSample(monoSample);
        const float detectedHz = freqDetector.getFrequency();
        dominantFrequency.store(detectedHz);

        if (isPitchValid(detectedHz))
        {
            lastValidFrequency = detectedHz;
            envelopeLevel = std::min(1.0f, envelopeLevel + attackStep);
        }
        else
        {
            envelopeLevel = std::max(0.0f, envelopeLevel - releaseStep);
        }

        float oscSample = 0.0f;
        if (blend > 0.0f && envelopeLevel > 0.0f)
        {
            oscillator.setFrequency(lastValidFrequency * pitchRatio);
            oscSample = oscillator.processSample() * oscGain * envelopeLevel;
        }

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
