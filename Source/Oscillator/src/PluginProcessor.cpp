#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
static const juce::String PARAM_FREQUENCY{"frequency"};
static const juce::String PARAM_WAVEFORM{"waveform"};
static const juce::String PARAM_GAIN{"gain"};

//==============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout
OscillatorAudioProcessor::createParameterLayout() {
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    // Frequency: 20 Hz – 20 kHz, default 440 Hz, skewed toward lower values
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        PARAM_FREQUENCY,
        "Frequency",
        juce::NormalisableRange<float>(20.0f, 20000.0f, 0.01f, 0.3f),
        440.0f,
        "Hz"
    ));

    // Waveform: Sine / Square / Saw / Triangle
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        PARAM_WAVEFORM,
        "Waveform",
        Oscillator::getWaveformNames(),
        0   // default: Sine
    ));

    // Output gain: 0..1, default 0.7
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        PARAM_GAIN,
        "Gain",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f),
        0.7f
    ));

    return layout;
}

//==============================================================================
OscillatorAudioProcessor::OscillatorAudioProcessor()
    : AudioProcessor(BusesProperties()
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "Parameters", createParameterLayout())
{
}

OscillatorAudioProcessor::~OscillatorAudioProcessor() {}

//==============================================================================
const juce::String OscillatorAudioProcessor::getName() const {
    return "JuceOscillator"; 
}

bool OscillatorAudioProcessor::acceptsMidi() const {
    return false;
}

bool OscillatorAudioProcessor::producesMidi() const {
    return false;
}

bool OscillatorAudioProcessor::isMidiEffect() const {
    return false;
}

double OscillatorAudioProcessor::getTailLengthSeconds() const {
    return 0.0;
}

int OscillatorAudioProcessor::getNumPrograms() {
    return 1;
}

int OscillatorAudioProcessor::getCurrentProgram() {
    return 0;
}

void OscillatorAudioProcessor::setCurrentProgram(int) {}

const juce::String OscillatorAudioProcessor::getProgramName(int) {
    return {};
}

void OscillatorAudioProcessor::changeProgramName(int, const juce::String&) {}

//==============================================================================
void OscillatorAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {
    oscillator.setSampleRate(sampleRate);
    oscillator.reset();

    smoothedGain.reset(sampleRate, 0.05);  // 50 ms smoothing
    smoothedGain.setCurrentAndTargetValue(apvts.getRawParameterValue(PARAM_GAIN)->load());
}

void OscillatorAudioProcessor::releaseResources() {}

bool OscillatorAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const {
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo()
        || layouts.getMainOutputChannelSet() == juce::AudioChannelSet::mono();
}

//==============================================================================
void OscillatorAudioProcessor::syncOscillatorParams() {
    const float freq = apvts.getRawParameterValue(PARAM_FREQUENCY)->load();
    const int wfIndex = static_cast<int>(apvts.getRawParameterValue(PARAM_WAVEFORM)->load());

    oscillator.setFrequency(freq);
    oscillator.setWaveform(Oscillator::waveformFromIndex(wfIndex));
}

void OscillatorAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& /*midiMessages*/) {
    juce::ScopedNoDenormals noDenormals;

    syncOscillatorParams();

    const float targetGain = apvts.getRawParameterValue(PARAM_GAIN)->load();
    smoothedGain.setTargetValue(targetGain);

    // Generate mono oscillator signal into channel 0
    oscillator.processBlock(buffer, 0);

    // Apply smoothed gain sample-by-sample on channel 0
    auto* ch0 = buffer.getWritePointer(0);
    for (int i = 0; i < buffer.getNumSamples(); ++i)
        ch0[i] *= smoothedGain.getNextValue();

    // Copy to remaining channels (stereo)
    for (int ch = 1; ch < buffer.getNumChannels(); ++ch)
        buffer.copyFrom(ch, 0, buffer, 0, 0, buffer.getNumSamples());
}

//==============================================================================
bool OscillatorAudioProcessor::hasEditor() const {
    return true;
}

juce::AudioProcessorEditor* OscillatorAudioProcessor::createEditor() {
    return new OscillatorAudioProcessorEditor(*this);
}

//==============================================================================
void OscillatorAudioProcessor::getStateInformation(juce::MemoryBlock& destData) {
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void OscillatorAudioProcessor::setStateInformation(const void* data, int sizeInBytes) {
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState != nullptr && xmlState->hasTagName(apvts.state.getType()))
        apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

//==============================================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
    return new OscillatorAudioProcessor();
}
