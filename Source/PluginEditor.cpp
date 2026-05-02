#include "PluginEditor.h"

namespace EditorConstants
{
    constexpr int windowWidth{ 600 };
    constexpr int windowHeight{ 400 };
}

ReHarmonizerAudioProcessorEditor::ReHarmonizerAudioProcessorEditor(ReHarmonizerAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    setSize(EditorConstants::windowWidth, EditorConstants::windowHeight);

    frequencyLabel.setText ("Oczekiwanie na sygnał...", juce::dontSendNotification);
    frequencyLabel.setJustificationType (juce::Justification::centred);
    frequencyLabel.setFont (juce::Font (30.0f, juce::Font::bold));
    frequencyLabel.setColour (juce::Label::textColourId, juce::Colours::white);

    addAndMakeVisible (frequencyLabel);

    startTimerHz (30);
}

ReHarmonizerAudioProcessorEditor::~ReHarmonizerAudioProcessorEditor()
{
    stopTimer();
}

void ReHarmonizerAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll (juce::Colours::darkgrey);
}

void ReHarmonizerAudioProcessorEditor::resized()
{
    frequencyLabel.setBounds (getLocalBounds().withSizeKeepingCentre (300, 50));
}

void ReHarmonizerAudioProcessorEditor::timerCallback()
{
    float currentFreq = audioProcessor.getDominantFrequency();

    juce::String freqText = juce::String (currentFreq, 1) + " Hz";

    frequencyLabel.setText (freqText, juce::dontSendNotification);
}