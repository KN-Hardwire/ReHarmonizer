#pragma once

#include "PluginProcessor.h"

class ReHarmonizerAudioProcessorEditor : public juce::AudioProcessorEditor, public juce::Timer
{
public:
    explicit ReHarmonizerAudioProcessorEditor(ReHarmonizerAudioProcessor&);
    ~ReHarmonizerAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

    void timerCallback() override;

private:
    ReHarmonizerAudioProcessor& audioProcessor;

    juce::Label frequencyLabel;

    juce::Slider blendKnob;
    juce::Slider pitchCorrectKnob;
    juce::Slider gainKnob;
    juce::ComboBox waveformSelector;

    juce::Label blendLabel;
    juce::Label pitchCorrectLabel;
    juce::Label gainLabel;
    juce::Label waveformLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ReHarmonizerAudioProcessorEditor)
};