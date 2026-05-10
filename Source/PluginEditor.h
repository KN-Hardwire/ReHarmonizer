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

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ReHarmonizerAudioProcessorEditor)
};
