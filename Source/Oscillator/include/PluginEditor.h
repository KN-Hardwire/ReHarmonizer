#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"

//==============================================================================
/**
 * Plugin editor with:
 *   - Rotary knob for Frequency
 *   - Rotary knob for Gain
 *   - ComboBox  for Waveform selection
 *   - Simple waveform visualiser
 */
class OscillatorAudioProcessorEditor : public juce::AudioProcessorEditor, private juce::Timer {
    public:
        explicit OscillatorAudioProcessorEditor(OscillatorAudioProcessor&);
        ~OscillatorAudioProcessorEditor() override;

        void paint(juce::Graphics&) override;
        void resized() override;

    private:
        //==========================================================================
        void timerCallback() override {
            repaint(); // refresh waveform preview
        }

        void drawWaveformPreview(juce::Graphics& g, juce::Rectangle<int> area);

        //==========================================================================
        OscillatorAudioProcessor& processor;

        // Controls
        juce::Slider frequencyKnob;
        juce::Slider gainKnob;
        juce::ComboBox waveformBox;

        juce::Label frequencyLabel;
        juce::Label gainLabel;
        juce::Label waveformLabel;

        // APVTS attachments
        using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
        using ComboAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

        std::unique_ptr<SliderAttachment> frequencyAttachment;
        std::unique_ptr<SliderAttachment> gainAttachment;
        std::unique_ptr<ComboAttachment> waveformAttachment;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OscillatorAudioProcessorEditor)
};
