#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "FrequencyDetector.h"
#include "Oscillator.h"

class ReHarmonizerAudioProcessor : public juce::AudioProcessor
{
public:
    ReHarmonizerAudioProcessor();
    ~ReHarmonizerAudioProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;


    float getDominantFrequency() const { return dominantFrequency.load(); }

    static constexpr const char* paramBlend = "blend";
    static constexpr const char* paramPitchCorrect = "pitchCorrect";
    static constexpr const char* paramGainDb = "gainDb";
    static constexpr const char* paramWaveform = "waveform";

	static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    juce::AudioProcessorValueTreeState apvts { *this,
											   nullptr,
											   "Parameters",
											   createParameterLayout() };

private:
	FrequencyDetector freqDetector;
	std::atomic<float> dominantFrequency { 0.0f };
	Oscillator oscillator;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ReHarmonizerAudioProcessor)
};
