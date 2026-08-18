#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <algorithm>
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

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(paramQuantizationLevel, 1),
        "Quantization",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.0f));

    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID(paramQuantizerKey, 1),
        "Key",
        juce::StringArray{ "C", "C#", "D", "D#", "E", "F",
                           "F#", "G", "G#", "A", "A#", "B" },
        static_cast<int>(PitchQuantizer::Key::C)));

    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID(paramScaleMode, 1),
        "Scale Mode",
        juce::StringArray{ "Major", "Minor" },
        static_cast<int>(PitchQuantizer::ScaleMode::Major)));

    return layout;
}

ReHarmonizerAudioProcessor::ReHarmonizerAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
	: AudioProcessor(BusesProperties()
		.withInput("Input", juce::AudioChannelSet::stereo(), true)
		.withOutput("Output", juce::AudioChannelSet::stereo(), true))
#endif
{
    clearWaveformDisplay();
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
    clearWaveformDisplay();
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
    const float quantizationLevel = juce::jlimit(
        0.0f, 1.0f, apvts.getRawParameterValue(paramQuantizationLevel)->load());
    const int quantizerKeyIndex = juce::jlimit(
        static_cast<int>(PitchQuantizer::Key::C),
        static_cast<int>(PitchQuantizer::Key::B),
        static_cast<int>(apvts.getRawParameterValue(paramQuantizerKey)->load()));
    const int scaleModeIndex = juce::jlimit(
        static_cast<int>(PitchQuantizer::ScaleMode::Major),
        static_cast<int>(PitchQuantizer::ScaleMode::Minor),
        static_cast<int>(apvts.getRawParameterValue(paramScaleMode)->load()));

    oscillator.setWaveform(static_cast<Oscillator::Waveform>(juce::jlimit(0, 3, waveformIndex)));
    pitchQuantizer.setQuantizationLevel(quantizationLevel);
    pitchQuantizer.setKey(static_cast<PitchQuantizer::Key>(quantizerKeyIndex));
    pitchQuantizer.setScaleMode(static_cast<PitchQuantizer::ScaleMode>(scaleModeIndex));

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
            pitchQuantizer.processFrequency(detectedHz);
            lastValidFrequency = pitchQuantizer.getQuantizedFrequency();
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

        float outputMonoSample = 0.0f;
        for (int channel = 0; channel < numChannels; ++channel)
        {
            auto* writePtr = buffer.getWritePointer(channel);
            const float outputSample = writePtr[sample] * dryMix + oscSample * blend;
            writePtr[sample] = outputSample;
            outputMonoSample += outputSample;
        }

        outputMonoSample /= static_cast<float>(numChannels);
        pushWaveformDisplaySample(monoSample, outputMonoSample);
    }
}

void ReHarmonizerAudioProcessor::copyWaveformDisplaySamples(
    std::array<float, waveformDisplayBufferSize>& inputSamples,
    std::array<float, waveformDisplayBufferSize>& outputSamples) const noexcept
{
    const auto writeCount = publishedWaveformWriteCounter.load(std::memory_order_acquire);
    const auto availableSamples = juce::jmin(writeCount, waveformDisplayBufferSize);
    const auto leadingSilence = waveformDisplayBufferSize - availableSamples;
    const auto firstSample = writeCount - availableSamples;

    std::fill_n(inputSamples.begin(), leadingSilence, 0.0f);
    std::fill_n(outputSamples.begin(), leadingSilence, 0.0f);

    for (std::size_t sample = 0; sample < availableSamples; ++sample)
    {
        const auto sourceIndex = (firstSample + sample) % waveformDisplayBufferSize;
        const auto destinationIndex = leadingSilence + sample;
        inputSamples[destinationIndex] = inputWaveformSamples[sourceIndex].load(std::memory_order_relaxed);
        outputSamples[destinationIndex] = outputWaveformSamples[sourceIndex].load(std::memory_order_relaxed);
    }
}

void ReHarmonizerAudioProcessor::clearWaveformDisplay() noexcept
{
    for (auto& sample : inputWaveformSamples)
        sample.store(0.0f, std::memory_order_relaxed);

    for (auto& sample : outputWaveformSamples)
        sample.store(0.0f, std::memory_order_relaxed);

    waveformWriteCounter = 0;
    publishedWaveformWriteCounter.store(0, std::memory_order_release);
}

void ReHarmonizerAudioProcessor::pushWaveformDisplaySample(float inputSample,
                                                            float outputSample) noexcept
{
    const auto destinationIndex = waveformWriteCounter % waveformDisplayBufferSize;
    inputWaveformSamples[destinationIndex].store(inputSample, std::memory_order_relaxed);
    outputWaveformSamples[destinationIndex].store(outputSample, std::memory_order_relaxed);
    publishedWaveformWriteCounter.store(++waveformWriteCounter, std::memory_order_release);
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
