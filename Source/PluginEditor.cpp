#include "PluginProcessor.h"
#include "PluginEditor.h"

namespace EditorConstants
{
    constexpr int windowWidth{ 720 };
    constexpr int windowHeight{ 430 };
}

ReHarmonizerAudioProcessorEditor::ReHarmonizerAudioProcessorEditor(ReHarmonizerAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    setSize(EditorConstants::windowWidth, EditorConstants::windowHeight);

    frequencyLabel.setText("Waiting for signal...", juce::dontSendNotification);
    frequencyLabel.setJustificationType (juce::Justification::centred);
    frequencyLabel.setFont(juce::Font(juce::FontOptions(30.0f, juce::Font::bold)));
    frequencyLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(frequencyLabel);

    startTimerHz(30);

    blendKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    blendKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible(blendKnob);

    blendLabel.setText("Blend", juce::dontSendNotification);
    blendLabel.setJustificationType(juce::Justification::centred);
    blendLabel.attachToComponent(&blendKnob, false);
    addAndMakeVisible(blendLabel);

    pitchCorrectKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    pitchCorrectKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible(pitchCorrectKnob);

    pitchCorrectLabel.setText("Pitch", juce::dontSendNotification);
    pitchCorrectLabel.setJustificationType(juce::Justification::centred);
    pitchCorrectLabel.attachToComponent(&pitchCorrectKnob, false);
    addAndMakeVisible(pitchCorrectLabel);

    gainKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    gainKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible(gainKnob);

    gainLabel.setText("Gain", juce::dontSendNotification);
    gainLabel.setJustificationType(juce::Justification::centred);
    gainLabel.attachToComponent(&gainKnob, false);
    addAndMakeVisible(gainLabel);

    attackKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    attackKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible(attackKnob);

    attackLabel.setText("Attack", juce::dontSendNotification);
    attackLabel.setJustificationType(juce::Justification::centred);
    attackLabel.attachToComponent(&attackKnob, false);
    addAndMakeVisible(attackLabel);

    releaseKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    releaseKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible(releaseKnob);

    releaseLabel.setText("Release", juce::dontSendNotification);
    releaseLabel.setJustificationType(juce::Justification::centred);
    releaseLabel.attachToComponent(&releaseKnob, false);
    addAndMakeVisible(releaseLabel);

    quantizationLevelKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    quantizationLevelKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible(quantizationLevelKnob);

    quantizationLevelLabel.setText("Quantization", juce::dontSendNotification);
    quantizationLevelLabel.setJustificationType(juce::Justification::centred);
    quantizationLevelLabel.attachToComponent(&quantizationLevelKnob, false);
    addAndMakeVisible(quantizationLevelLabel);

    waveformSelector.addItem("Sine", 1);
    waveformSelector.addItem("Square", 2);
    waveformSelector.addItem("Sawtooth", 3);
    waveformSelector.addItem("Triangle", 4);
    addAndMakeVisible(waveformSelector);

    waveformLabel.setText("Waveform", juce::dontSendNotification);
    waveformLabel.attachToComponent(&waveformSelector, false);
    addAndMakeVisible(waveformLabel);

    keySelector.addItemList({ "C", "C#", "D", "D#", "E", "F",
                              "F#", "G", "G#", "A", "A#", "B" }, 1);
    addAndMakeVisible(keySelector);

    keyLabel.setText("Key", juce::dontSendNotification);
    keyLabel.attachToComponent(&keySelector, false);
    addAndMakeVisible(keyLabel);

    scaleModeSelector.addItemList({ "Major", "Minor" }, 1);
    addAndMakeVisible(scaleModeSelector);

    scaleModeLabel.setText("Scale Mode", juce::dontSendNotification);
    scaleModeLabel.attachToComponent(&scaleModeSelector, false);
    addAndMakeVisible(scaleModeLabel);

    blendAttachment = std::make_unique<SliderAttachment>(audioProcessor.apvts,
                                                         ReHarmonizerAudioProcessor::paramBlend,
                                                         blendKnob);
    pitchCorrectAttachment = std::make_unique<SliderAttachment>(audioProcessor.apvts,
                                                                ReHarmonizerAudioProcessor::paramPitchCorrect,
                                                                pitchCorrectKnob);
    gainAttachment = std::make_unique<SliderAttachment>(audioProcessor.apvts,
                                                        ReHarmonizerAudioProcessor::paramGainDb,
                                                        gainKnob);
    attackAttachment = std::make_unique<SliderAttachment>(audioProcessor.apvts,
                                                          ReHarmonizerAudioProcessor::paramAttack,
                                                          attackKnob);
    releaseAttachment = std::make_unique<SliderAttachment>(audioProcessor.apvts,
                                                           ReHarmonizerAudioProcessor::paramRelease,
                                                           releaseKnob);
    quantizationLevelAttachment = std::make_unique<SliderAttachment>(
        audioProcessor.apvts,
        ReHarmonizerAudioProcessor::paramQuantizationLevel,
        quantizationLevelKnob);
    waveformAttachment = std::make_unique<ComboBoxAttachment>(audioProcessor.apvts,
                                                              ReHarmonizerAudioProcessor::paramWaveform,
                                                              waveformSelector);
    keyAttachment = std::make_unique<ComboBoxAttachment>(audioProcessor.apvts,
                                                         ReHarmonizerAudioProcessor::paramQuantizerKey,
                                                         keySelector);
    scaleModeAttachment = std::make_unique<ComboBoxAttachment>(audioProcessor.apvts,
                                                               ReHarmonizerAudioProcessor::paramScaleMode,
                                                               scaleModeSelector);
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
    
    area.reduce(25, 30);

    constexpr int knobSize = 95;
    constexpr int spacing = 20;

    blendKnob.setBounds(area.getX(), area.getY(), knobSize, knobSize);
    pitchCorrectKnob.setBounds(blendKnob.getRight() + spacing, area.getY(), knobSize, knobSize);
    gainKnob.setBounds(pitchCorrectKnob.getRight() + spacing, area.getY(), knobSize, knobSize);
    attackKnob.setBounds(gainKnob.getRight() + spacing, area.getY(), knobSize, knobSize);
    releaseKnob.setBounds(attackKnob.getRight() + spacing, area.getY(), knobSize, knobSize);
    quantizationLevelKnob.setBounds(releaseKnob.getRight() + spacing,
                                    area.getY(),
                                    knobSize,
                                    knobSize);

    constexpr int selectorWidth = 190;
    constexpr int selectorHeight = 30;
    constexpr int selectorSpacing = 35;
    const int selectorY = blendKnob.getBottom() + 55;

    waveformSelector.setBounds(area.getX(), selectorY, selectorWidth, selectorHeight);
    keySelector.setBounds(waveformSelector.getRight() + selectorSpacing,
                          selectorY,
                          selectorWidth,
                          selectorHeight);
    scaleModeSelector.setBounds(keySelector.getRight() + selectorSpacing,
                                selectorY,
                                selectorWidth,
                                selectorHeight);
}

void ReHarmonizerAudioProcessorEditor::timerCallback()
{
    float currentFreq = audioProcessor.getDominantFrequency();

    juce::String freqText = juce::String(currentFreq, 1) + " Hz";

    frequencyLabel.setText(freqText, juce::dontSendNotification);
}
