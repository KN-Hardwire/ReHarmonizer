#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "Oscillator.h"

//==============================================================================
class OscillatorAudioProcessor : public juce::AudioProcessor {
    public:
        //==========================================================================
        OscillatorAudioProcessor();
        ~OscillatorAudioProcessor() override;

        //==========================================================================
        void prepareToPlay(double sampleRate, int samplesPerBlock) override;
        void releaseResources() override;

        bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

        void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

        //==========================================================================
        juce::AudioProcessorEditor* createEditor() override;
        bool hasEditor() const override;

        //==========================================================================
        const juce::String getName() const override;

        bool acceptsMidi() const override;
        bool producesMidi() const override;
        bool isMidiEffect() const override;
        double getTailLengthSeconds() const override;

        //==========================================================================
        int getNumPrograms() override;
        int getCurrentProgram() override;
        void setCurrentProgram(int index) override;
        const juce::String getProgramName(int index) override;
        void changeProgramName(int index, const juce::String& newName) override;

        //==========================================================================
        void getStateInformation(juce::MemoryBlock& destData) override;
        void setStateInformation(const void* data, int sizeInBytes) override;

        //==========================================================================
        juce::AudioProcessorValueTreeState& getAPVTS() { 
           return apvts;
        }

    private:
        //==========================================================================
        juce::AudioProcessorValueTreeState apvts;
        static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

        Oscillator oscillator;

        // Smooth gain to prevent clicks on parameter changes
        juce::LinearSmoothedValue<float> smoothedGain;

        void syncOscillatorParams();

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OscillatorAudioProcessor)
};
