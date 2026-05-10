#include "PluginProcessor.h"
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

    frequencyLabel.setText ("Waiting for signal...", juce::dontSendNotification);
    frequencyLabel.setJustificationType (juce::Justification::centred);
    frequencyLabel.setFont (juce::Font (30.0f, juce::Font::bold));
    frequencyLabel.setColour (juce::Label::textColourId, juce::Colours::white);

    startTimerHz(30);

    // Twój kod: Inicjalizacja komponentów z taska #7
    blendKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    blendKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    blendKnob.setRange(0.0, 1.0, 0.01);
    blendKnob.setValue(0.5);
    addAndMakeVisible(blendKnob);

    blendLabel.setText("Blend", juce::dontSendNotification);
    blendLabel.setJustificationType(juce::Justification::centred);
    blendLabel.attachToComponent(&blendKnob, false);
    addAndMakeVisible(blendLabel);

    pitchCorrectKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    pitchCorrectKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    pitchCorrectKnob.setRange(-12.0, 12.0, 0.1);
    pitchCorrectKnob.setValue(0.0);
    addAndMakeVisible(pitchCorrectKnob);

    pitchCorrectLabel.setText("Pitch", juce::dontSendNotification);
    pitchCorrectLabel.setJustificationType(juce::Justification::centred);
    pitchCorrectLabel.attachToComponent(&pitchCorrectKnob, false);
    addAndMakeVisible(pitchCorrectLabel);

    gainKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    gainKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    gainKnob.setRange(-24.0, 24.0, 0.1);
    gainKnob.setValue(0.0);
    addAndMakeVisible(gainKnob);

    gainLabel.setText("Gain", juce::dontSendNotification);
    gainLabel.setJustificationType(juce::Justification::centred);
    gainLabel.attachToComponent(&gainKnob, false);
    addAndMakeVisible(gainLabel);

    waveformSelector.addItem("Sine", 1);
    waveformSelector.addItem("Square", 2);
    waveformSelector.addItem("Sawtooth", 3);
    waveformSelector.addItem("Triangle", 4);
    waveformSelector.setSelectedId(1);
    addAndMakeVisible(waveformSelector);

    waveformLabel.setText("Waveform", juce::dontSendNotification);
    waveformLabel.attachToComponent(&waveformSelector, false);
    addAndMakeVisible(waveformLabel);
}

ReHarmonizerAudioProcessorEditor::~ReHarmonizerAudioProcessorEditor()
{
    stopTimer();
}

void ReHarmonizerAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey); 
}

void ReHarmonizerAudioProcessorEditor::resized()
{
    auto area = getLocalBounds();
    
   
    frequencyLabel.setBounds(area.removeFromTop(100).withSizeKeepingCentre(300, 50));

    
    area.reduce(20, 40);

    int knobSize = 100;
    int spacing = 20;

    blendKnob.setBounds(area.getX(), area.getY(), knobSize, knobSize);
    pitchCorrectKnob.setBounds(blendKnob.getRight() + spacing, area.getY(), knobSize, knobSize);
    gainKnob.setBounds(pitchCorrectKnob.getRight() + spacing, area.getY(), knobSize, knobSize);

    waveformSelector.setBounds(area.getX(), blendKnob.getBottom() + 40, 150, 30);
}

void ReHarmonizerAudioProcessorEditor::timerCallback()
{
    float currentFreq = audioProcessor.getDominantFrequency();

    juce::String freqText = juce::String (currentFreq, 1) + " Hz";

    frequencyLabel.setText (freqText, juce::dontSendNotification);
}
