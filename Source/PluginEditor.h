#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class ReHarmonizerAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    ReHarmonizerAudioProcessorEditor (ReHarmonizerAudioProcessor&);
    ~ReHarmonizerAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    ReHarmonizerAudioProcessor& audioProcessor;

    juce::Slider blendKnob;
    juce::Slider pitchCorrectKnob;
    juce::Slider gainKnob;
    juce::ComboBox waveformSelector;

    juce::Label blendLabel;
    juce::Label pitchCorrectLabel;
    juce::Label gainLabel;
    juce::Label waveformLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ReHarmonizerAudioProcessorEditor)
};