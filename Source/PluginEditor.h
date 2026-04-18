#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"

class ReHarmonizerAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
	explicit ReHarmonizerAudioProcessorEditor(ReHarmonizerAudioProcessor&);
	~ReHarmonizerAudioProcessorEditor() override;

	void paint(juce::Graphics&) override;
	void resized() override;

private:
	ReHarmonizerAudioProcessor& audioProcessor;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ReHarmonizerAudioProcessorEditor)
};
