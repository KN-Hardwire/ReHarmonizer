#pragma once

#include "PluginProcessor.h"
#include <memory>

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
    juce::Slider attackKnob;
    juce::Slider releaseKnob;
    juce::Slider quantizationLevelKnob;
    juce::ComboBox waveformSelector;
    juce::ComboBox keySelector;
    juce::ComboBox scaleModeSelector;

    juce::Label blendLabel;
    juce::Label pitchCorrectLabel;
    juce::Label gainLabel;
    juce::Label attackLabel;
    juce::Label releaseLabel;
    juce::Label quantizationLevelLabel;
    juce::Label waveformLabel;
    juce::Label keyLabel;
    juce::Label scaleModeLabel;

    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ComboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

    std::unique_ptr<SliderAttachment> blendAttachment;
    std::unique_ptr<SliderAttachment> pitchCorrectAttachment;
    std::unique_ptr<SliderAttachment> gainAttachment;
    std::unique_ptr<SliderAttachment> attackAttachment;
    std::unique_ptr<SliderAttachment> releaseAttachment;
    std::unique_ptr<SliderAttachment> quantizationLevelAttachment;
    std::unique_ptr<ComboBoxAttachment> waveformAttachment;
    std::unique_ptr<ComboBoxAttachment> keyAttachment;
    std::unique_ptr<ComboBoxAttachment> scaleModeAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ReHarmonizerAudioProcessorEditor)
};
