#include "PluginEditor.h"
#include "Oscillator.h"
#include <cmath>

//==============================================================================
static void setupRotaryKnob(juce::Slider& slider) {
    slider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    slider.setColour(juce::Slider::rotarySliderFillColourId,   juce::Colour(0xff4fc3f7));
    slider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xff1a3a5c));
    slider.setColour(juce::Slider::thumbColourId,               juce::Colours::white);
}

static void setupLabel(juce::Label& label, const juce::String& text) {
    label.setText(text, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    label.setFont(juce::Font(13.0f, juce::Font::bold));
    label.setColour(juce::Label::textColourId, juce::Colours::lightblue);
}

//==============================================================================
OscillatorAudioProcessorEditor::OscillatorAudioProcessorEditor(OscillatorAudioProcessor& p) : AudioProcessorEditor(&p), processor(p) {
    setSize(420, 340);

    // ---- Frequency knob ----
    setupRotaryKnob(frequencyKnob);
    frequencyKnob.setSkewFactorFromMidPoint(1000.0);
    addAndMakeVisible(frequencyKnob);
    setupLabel(frequencyLabel, "Frequency");
    addAndMakeVisible(frequencyLabel);

    frequencyAttachment = std::make_unique<SliderAttachment>(processor.getAPVTS(), "frequency", frequencyKnob);

    // ---- Gain knob ----
    setupRotaryKnob(gainKnob);
    addAndMakeVisible(gainKnob);
    setupLabel(gainLabel, "Gain");
    addAndMakeVisible(gainLabel);

    gainAttachment = std::make_unique<SliderAttachment>(processor.getAPVTS(), "gain", gainKnob);

    // ---- Waveform ComboBox ----
    const auto waveformNames = Oscillator::getWaveformNames();
    for (int i = 0; i < waveformNames.size(); ++i)
        waveformBox.addItem(waveformNames[i], i + 1);

    waveformBox.setColour(juce::ComboBox::backgroundColourId, juce::Colour(0xff0d2137));
    waveformBox.setColour(juce::ComboBox::textColourId, juce::Colours::lightblue);
    waveformBox.setColour(juce::ComboBox::outlineColourId, juce::Colour(0xff4fc3f7));
    addAndMakeVisible(waveformBox);

    setupLabel(waveformLabel, "Waveform");
    addAndMakeVisible(waveformLabel);

    waveformAttachment = std::make_unique<ComboAttachment>(processor.getAPVTS(), "waveform", waveformBox);

    // Refresh waveform preview at 30 fps
    startTimerHz(30);
}

OscillatorAudioProcessorEditor::~OscillatorAudioProcessorEditor() {
    stopTimer();
}

//==============================================================================
void OscillatorAudioProcessorEditor::paint(juce::Graphics& g) {
    // Background gradient
    g.fillAll(juce::Colour(0xff0a1929));

    // Title
    g.setFont(juce::Font(20.0f, juce::Font::bold));
    g.setColour(juce::Colour(0xff4fc3f7));
    g.drawText("JUCE Oscillator", getLocalBounds().removeFromTop(36), juce::Justification::centred, true);

    // Waveform preview panel
    auto previewArea = getLocalBounds().removeFromBottom(120).reduced(16, 8);
    g.setColour(juce::Colour(0xff0d2137));
    g.fillRoundedRectangle(previewArea.toFloat(), 8.0f);
    g.setColour(juce::Colour(0xff1a3a5c));
    g.drawRoundedRectangle(previewArea.toFloat(), 8.0f, 1.5f);

    drawWaveformPreview(g, previewArea.reduced(8));
}

//==============================================================================
void OscillatorAudioProcessorEditor::drawWaveformPreview(juce::Graphics& g, juce::Rectangle<int> area) {
    const int   wfIndex = static_cast<int>(processor.getAPVTS().getRawParameterValue("waveform")->load());
    const auto  waveform = Oscillator::waveformFromIndex(wfIndex);
    const float midY = area.getCentreY();
    const float halfH = area.getHeight() * 0.45f;
    const float width = static_cast<float>(area.getWidth());
    constexpr float TWO_PI = juce::MathConstants<float>::twoPi;

    // Grid lines
    g.setColour(juce::Colour(0xff1a3a5c));
    g.drawHorizontalLine(static_cast<int>(midY), area.getX(), area.getRight());

    // Waveform path (2 full cycles)
    juce::Path path;
    const int steps = area.getWidth();

    auto sampleWaveform = [waveform](float phase) -> float
    {
        switch (waveform) { 
            case Oscillator::Waveform::Sine:
                return std::sin(phase);

            case Oscillator::Waveform::Square:
                return (phase < juce::MathConstants<float>::pi) ? 1.0f : -1.0f;

            case Oscillator::Waveform::Saw:
                return (phase / juce::MathConstants<float>::pi) - 1.0f;

            case Oscillator::Waveform::Triangle:
            {
                const float norm = phase / TWO_PI;
                if (norm < 0.25f) 
                    return norm * 4.0f;
                else if (norm < 0.75f)
                    return 1.0f - (norm - 0.25f) * 4.0f;
                else
                    return -1.0f + (norm - 0.75f) * 4.0f;
            }
            default: return 0.0f;
        }
    };

    for (int x = 0; x < steps; ++x) {
        const float phase = TWO_PI * 2.0f * (static_cast<float>(x) / width);
        const float normPhase = std::fmod(phase, TWO_PI);
        const float y = midY - sampleWaveform(normPhase) * halfH;

        if (x == 0)
            path.startNewSubPath(area.getX() + x, y);
        else
            path.lineTo(area.getX() + x, y);
    }

    g.setColour(juce::Colour(0xff4fc3f7));
    g.strokePath(path, juce::PathStrokeType(2.0f));

    // Waveform name overlay
    g.setFont(juce::Font(12.0f));
    g.setColour(juce::Colour(0x884fc3f7));
    g.drawText(Oscillator::getWaveformNames()[wfIndex], area, juce::Justification::bottomRight, true);
}

//==============================================================================
void OscillatorAudioProcessorEditor::resized() {
    auto bounds = getLocalBounds();
    bounds.removeFromTop(36);          // title
    bounds.removeFromBottom(120);      // waveform preview

    auto controlArea = bounds.reduced(10, 4);

    // Waveform selector row
    auto waveformRow = controlArea.removeFromTop(56);
    waveformLabel.setBounds(waveformRow.removeFromLeft(80));
    waveformBox.setBounds(waveformRow.reduced(4, 10));

    // Knobs row
    const int knobW = controlArea.getWidth() / 2;
    auto freqArea = controlArea.removeFromLeft(knobW);
    auto gainArea = controlArea;

    const int labelH = 20;
    frequencyLabel.setBounds(freqArea.removeFromTop(labelH));
    frequencyKnob.setBounds(freqArea.reduced(10, 0));

    gainLabel.setBounds(gainArea.removeFromTop(labelH));
    gainKnob.setBounds(gainArea.reduced(10, 0));
}
