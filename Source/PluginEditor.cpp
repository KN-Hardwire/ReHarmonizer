#include "PluginProcessor.h"
#include "PluginEditor.h"

#include <cmath>

namespace
{
namespace Palette
{
const juce::Colour appBackground { juce::Colour::fromRGB(230, 233, 235) };
const juce::Colour canvas { juce::Colour::fromRGB(77, 77, 84) };
const juce::Colour panel { juce::Colour::fromRGB(190, 191, 196) };
const juce::Colour displayPanel { juce::Colour::fromRGB(166, 168, 181) };
const juce::Colour knob { juce::Colour::fromRGB(51, 54, 66) };
const juce::Colour knobTrack { juce::Colour::fromRGB(127, 129, 143) };
const juce::Colour purple { juce::Colour::fromRGB(72, 67, 99) };
const juce::Colour accent { juce::Colour::fromRGB(34, 205, 195) };
const juce::Colour ink { juce::Colour::fromRGB(31, 33, 39) };
const juce::Colour mutedInk { juce::Colour::fromRGB(100, 111, 118) };
}

constexpr int windowWidth { 900 };
constexpr int windowHeight { 600 };
constexpr int titleBarHeight { 45 };
constexpr int canvasInset { 9 };
constexpr int contentInset { 17 };
constexpr int panelGap { 13 };
constexpr float panelCornerRadius { 3.0f };

void drawPanel(juce::Graphics& g, juce::Rectangle<int> bounds, juce::Colour colour)
{
    g.setColour(colour);
    g.fillRoundedRectangle(bounds.toFloat(), panelCornerRadius);
}

void drawSectionTitle(juce::Graphics& g, const juce::String& title, juce::Rectangle<int> bounds)
{
    g.setColour(Palette::ink.withAlpha(0.78f));
    g.setFont(juce::Font(juce::FontOptions(14.0f, juce::Font::bold)));
    g.drawText(title, bounds, juce::Justification::centredLeft, false);
}
}

ReHarmonizerLookAndFeel::ReHarmonizerLookAndFeel()
{
    setColour(juce::Slider::textBoxTextColourId, Palette::ink);
    setColour(juce::Slider::textBoxBackgroundColourId, Palette::panel.brighter(0.08f));
    setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    setColour(juce::Label::textColourId, Palette::ink);
    setColour(juce::ComboBox::backgroundColourId, Palette::purple);
    setColour(juce::ComboBox::textColourId, juce::Colours::white);
    setColour(juce::ComboBox::outlineColourId, juce::Colours::transparentBlack);
    setColour(juce::ComboBox::arrowColourId, Palette::accent);
    setColour(juce::PopupMenu::backgroundColourId, Palette::knob);
    setColour(juce::PopupMenu::textColourId, juce::Colours::white);
    setColour(juce::PopupMenu::highlightedBackgroundColourId, Palette::purple);
    setColour(juce::PopupMenu::highlightedTextColourId, juce::Colours::white);
}

void ReHarmonizerLookAndFeel::drawRotarySlider(juce::Graphics& g,
                                                int x,
                                                int y,
                                                int width,
                                                int height,
                                                float sliderPos,
                                                float rotaryStartAngle,
                                                float rotaryEndAngle,
                                                juce::Slider&)
{
    const auto available = juce::Rectangle<float>(static_cast<float>(x),
                                                   static_cast<float>(y),
                                                   static_cast<float>(width),
                                                   static_cast<float>(height)).reduced(7.0f);
    const float radius = juce::jmin(available.getWidth(), available.getHeight()) * 0.5f;
    const auto centre = available.getCentre();
    const auto knobBounds = juce::Rectangle<float>(radius * 2.0f, radius * 2.0f)
                                .withCentre(centre);

    g.setColour(Palette::ink.withAlpha(0.08f));
    g.fillEllipse(knobBounds.translated(4.0f, 5.0f));

    g.setColour(Palette::knob);
    g.fillEllipse(knobBounds);

    juce::Path track;
    track.addCentredArc(centre.x,
                        centre.y,
                        radius * 0.86f,
                        radius * 0.86f,
                        0.0f,
                        rotaryStartAngle,
                        rotaryEndAngle,
                        true);
    g.setColour(Palette::knobTrack);
    g.strokePath(track, juce::PathStrokeType(2.5f));

    const float angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    juce::Path valueArc;
    valueArc.addCentredArc(centre.x,
                           centre.y,
                           radius * 0.86f,
                           radius * 0.86f,
                           0.0f,
                           rotaryStartAngle,
                           angle,
                           true);
    g.setColour(Palette::accent);
    g.strokePath(valueArc, juce::PathStrokeType(2.5f));

    const auto pointerEnd = juce::Point<float>(centre.x + std::sin(angle) * radius * 0.74f,
                                               centre.y - std::cos(angle) * radius * 0.74f);
    g.setColour(Palette::panel);
    g.drawLine({ centre, pointerEnd }, 2.9f);
}

void ReHarmonizerLookAndFeel::drawComboBox(juce::Graphics& g,
                                           int width,
                                           int height,
                                           bool isButtonDown,
                                           int buttonX,
                                           int buttonY,
                                           int buttonWidth,
                                           int buttonHeight,
                                           juce::ComboBox& box)
{
    const auto bounds = juce::Rectangle<float>(0.0f,
                                               0.0f,
                                               static_cast<float>(width),
                                               static_cast<float>(height));
    g.setColour(box.findColour(juce::ComboBox::backgroundColourId)
                   .brighter(isButtonDown ? 0.12f : 0.0f));
    g.fillRoundedRectangle(bounds, 2.0f);

    if (box.hasKeyboardFocus(true))
    {
        g.setColour(Palette::accent);
        g.drawRoundedRectangle(bounds.reduced(0.5f), 2.0f, 1.0f);
    }

    const auto buttonArea = juce::Rectangle<float>(static_cast<float>(buttonX),
                                                   static_cast<float>(buttonY),
                                                   static_cast<float>(buttonWidth),
                                                   static_cast<float>(buttonHeight));
    const auto centre = buttonArea.getCentre();
    juce::Path arrow;
    arrow.startNewSubPath(centre.x - 5.0f, centre.y - 2.0f);
    arrow.lineTo(centre.x, centre.y + 3.0f);
    arrow.lineTo(centre.x + 5.0f, centre.y - 2.0f);

    g.setColour(box.findColour(juce::ComboBox::arrowColourId));
    g.strokePath(arrow, juce::PathStrokeType(2.0f));
}

void ReHarmonizerLookAndFeel::positionComboBoxText(juce::ComboBox& box, juce::Label& label)
{
    label.setBounds(12, 1, box.getWidth() - 42, box.getHeight() - 2);
    label.setFont(juce::Font(juce::FontOptions(15.0f)));
    label.setJustificationType(juce::Justification::centredLeft);
}

ReHarmonizerAudioProcessorEditor::ReHarmonizerAudioProcessorEditor(ReHarmonizerAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    setLookAndFeel(&lookAndFeel);
    setSize(windowWidth, windowHeight);

    frequencyLabel.setText("No pitch", juce::dontSendNotification);
    frequencyLabel.setJustificationType(juce::Justification::centred);
    frequencyLabel.setFont(juce::Font(juce::FontOptions(18.0f, juce::Font::bold)));
    frequencyLabel.setColour(juce::Label::backgroundColourId, Palette::appBackground.withAlpha(0.78f));
    frequencyLabel.setColour(juce::Label::textColourId, Palette::ink);
    addAndMakeVisible(frequencyLabel);

    const auto configureKnob = [this](juce::Slider& slider,
                                      juce::Label& label,
                                      const juce::String& text)
    {
        slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 21);
        addAndMakeVisible(slider);

        label.setText(text, juce::dontSendNotification);
        label.setJustificationType(juce::Justification::centred);
        label.setFont(juce::Font(juce::FontOptions(15.0f)));
        addAndMakeVisible(label);
    };

    configureKnob(blendKnob, blendLabel, "Blend");
    configureKnob(pitchCorrectKnob, pitchCorrectLabel, "Pitch");
    configureKnob(gainKnob, gainLabel, "Gain");
    configureKnob(attackKnob, attackLabel, "Attack");
    configureKnob(releaseKnob, releaseLabel, "Release");
    configureKnob(quantizationLevelKnob, quantizationLevelLabel, "Quantization level");

    const auto configureSelector = [this](juce::ComboBox& selector,
                                          juce::Label& label,
                                          const juce::String& text)
    {
        addAndMakeVisible(selector);
        label.setText(text, juce::dontSendNotification);
        label.setJustificationType(juce::Justification::centredLeft);
        label.setFont(juce::Font(juce::FontOptions(13.0f, juce::Font::bold)));
        addAndMakeVisible(label);
    };

    waveformSelector.addItemList({ "Sine", "Square", "Sawtooth", "Triangle" }, 1);
    configureSelector(waveformSelector, waveformLabel, "WAVEFORM");

    keySelector.addItemList({ "C", "C#", "D", "D#", "E", "F",
                              "F#", "G", "G#", "A", "A#", "B" }, 1);
    configureSelector(keySelector, keyLabel, "KEY");

    scaleModeSelector.addItemList({ "Major", "Minor" }, 1);
    configureSelector(scaleModeSelector, scaleModeLabel, "SCALE MODE");

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

    startTimerHz(30);
}

ReHarmonizerAudioProcessorEditor::~ReHarmonizerAudioProcessorEditor()
{
    stopTimer();
    setLookAndFeel(nullptr);
}

void ReHarmonizerAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(Palette::appBackground);

    g.setColour(Palette::ink);
    g.setFont(juce::Font(juce::FontOptions(20.0f, juce::Font::bold)));
    g.drawText("ReHarmonizer", 17, 7, 152, 30,
               juce::Justification::centredLeft, false);

    g.setColour(Palette::mutedInk.withAlpha(0.54f));
    g.setFont(juce::Font(juce::FontOptions(13.5f)));
    g.drawText("by Hardwire", 165, 8, 107, 30,
               juce::Justification::centredLeft, false);

    const auto canvasBounds = getLocalBounds().withTrimmedTop(titleBarHeight).reduced(canvasInset);
    drawPanel(g, canvasBounds, Palette::canvas);
    drawPanel(g, waveformPanelBounds, Palette::displayPanel);
    drawPanel(g, mainControlsPanelBounds, Palette::panel);
    drawPanel(g, envelopePanelBounds, Palette::panel);
    drawPanel(g, pitchCorrectionPanelBounds, Palette::panel);

    g.setColour(Palette::accent);
    g.drawRoundedRectangle(waveformPanelBounds.toFloat(), panelCornerRadius, 1.4f);

    drawSectionTitle(g, "WAVEFORM", waveformPanelBounds.reduced(18).removeFromTop(28));
    drawSectionTitle(g, "OUTPUT & TONE", mainControlsPanelBounds.reduced(16).removeFromTop(25));
    drawSectionTitle(g, "ENVELOPE", envelopePanelBounds.reduced(18).removeFromTop(28));
    drawSectionTitle(g, "PITCH CORRECTION", pitchCorrectionPanelBounds.reduced(18).removeFromTop(28));

    drawWaveformDisplay(g);
    drawEnvelopeDisplay(g);
}

void ReHarmonizerAudioProcessorEditor::resized()
{
    auto content = getLocalBounds().withTrimmedTop(titleBarHeight).reduced(contentInset);
    const int leftWidth = static_cast<int>(std::round((content.getWidth() - panelGap) * 0.56));

    auto leftColumn = content.removeFromLeft(leftWidth);
    content.removeFromLeft(panelGap);
    auto rightColumn = content;

    const int topPanelHeight = static_cast<int>(
        std::round((content.getHeight() - panelGap) * 0.44));
    waveformPanelBounds = leftColumn.removeFromTop(topPanelHeight);
    leftColumn.removeFromTop(panelGap);
    envelopePanelBounds = leftColumn;

    mainControlsPanelBounds = rightColumn.removeFromTop(topPanelHeight);
    rightColumn.removeFromTop(panelGap);
    pitchCorrectionPanelBounds = rightColumn;

    auto waveformContent = waveformPanelBounds.reduced(18);
    auto waveformHeader = waveformContent.removeFromTop(40);
    frequencyLabel.setBounds(waveformHeader.removeFromRight(180));
    waveformGraphBounds = waveformContent.reduced(8, 12);

    auto mainControls = mainControlsPanelBounds.reduced(16);
    mainControls.removeFromTop(25);
    auto waveformSelectorArea = mainControls.removeFromBottom(54);
    waveformLabel.setBounds(waveformSelectorArea.removeFromTop(19));
    waveformSelector.setBounds(waveformSelectorArea.removeFromTop(33));

    const int controlWidth = mainControls.getWidth() / 3;
    auto placeKnob = [](juce::Rectangle<int> column,
                        juce::Slider& knob,
                        juce::Label& label)
    {
        label.setBounds(column.removeFromBottom(21));
        knob.setBounds(column.reduced(3, 0));
    };

    auto blendArea = mainControls.removeFromLeft(controlWidth);
    auto pitchArea = mainControls.removeFromLeft(controlWidth);
    placeKnob(blendArea, blendKnob, blendLabel);
    placeKnob(pitchArea, pitchCorrectKnob, pitchCorrectLabel);
    placeKnob(mainControls, gainKnob, gainLabel);

    auto envelopeContent = envelopePanelBounds.reduced(18);
    envelopeContent.removeFromTop(31);
    auto envelopeKnobs = envelopeContent.removeFromBottom(132);
    envelopeGraphBounds = envelopeContent.reduced(5, 7);
    const int envelopeControlWidth = envelopeKnobs.getWidth() / 2;
    auto attackArea = envelopeKnobs.removeFromLeft(envelopeControlWidth).reduced(25, 0);
    auto releaseArea = envelopeKnobs.reduced(25, 0);
    placeKnob(attackArea, attackKnob, attackLabel);
    placeKnob(releaseArea, releaseKnob, releaseLabel);

    auto pitchCorrectionContent = pitchCorrectionPanelBounds.reduced(18);
    pitchCorrectionContent.removeFromTop(31);
    auto selectorsArea = pitchCorrectionContent.removeFromBottom(63);
    auto quantizationLabelArea = pitchCorrectionContent.removeFromBottom(22);
    quantizationLevelLabel.setBounds(quantizationLabelArea);
    quantizationLevelKnob.setBounds(pitchCorrectionContent.withSizeKeepingCentre(126, 126));

    constexpr int selectorGap = 12;
    const int selectorWidth = (selectorsArea.getWidth() - selectorGap) / 2;
    auto keyArea = selectorsArea.removeFromLeft(selectorWidth);
    selectorsArea.removeFromLeft(selectorGap);
    auto scaleArea = selectorsArea;

    keyLabel.setBounds(keyArea.removeFromTop(19));
    keySelector.setBounds(keyArea.removeFromTop(34));
    scaleModeLabel.setBounds(scaleArea.removeFromTop(19));
    scaleModeSelector.setBounds(scaleArea.removeFromTop(34));
}

void ReHarmonizerAudioProcessorEditor::timerCallback()
{
    const float currentFrequency = audioProcessor.getDominantFrequency();
    const auto text = currentFrequency > 0.0f
        ? juce::String(currentFrequency, 1) + " Hz"
        : juce::String("No pitch");

    frequencyLabel.setText(text, juce::dontSendNotification);
    audioProcessor.copyWaveformDisplaySamples(inputWaveformDisplay, outputWaveformDisplay);
    repaint();
}

void ReHarmonizerAudioProcessorEditor::drawWaveformDisplay(juce::Graphics& g) const
{
    if (waveformGraphBounds.isEmpty())
        return;

    const auto graph = waveformGraphBounds.toFloat();
    g.setColour(Palette::ink.withAlpha(0.12f));
    g.drawHorizontalLine(static_cast<int>(graph.getCentreY()), graph.getX(), graph.getRight());

    float peakMagnitude = 0.05f;
    for (std::size_t sample = 0; sample < inputWaveformDisplay.size(); ++sample)
    {
        peakMagnitude = juce::jmax(peakMagnitude, std::abs(inputWaveformDisplay[sample]));
        peakMagnitude = juce::jmax(peakMagnitude, std::abs(outputWaveformDisplay[sample]));
    }

    const auto makeWaveformPath = [&graph, peakMagnitude](const auto& samples)
    {
        juce::Path path;
        for (std::size_t sample = 0; sample < samples.size(); ++sample)
        {
            const float progress = static_cast<float>(sample)
                / static_cast<float>(samples.size() - 1);
            const float value = juce::jlimit(-peakMagnitude, peakMagnitude, samples[sample]);
            const float x = graph.getX() + progress * graph.getWidth();
            const float y = graph.getCentreY()
                - (value / peakMagnitude) * graph.getHeight() * 0.38f;

            if (sample == 0)
                path.startNewSubPath(x, y);
            else
                path.lineTo(x, y);
        }
        return path;
    };

    const auto inputPath = makeWaveformPath(inputWaveformDisplay);
    const auto outputPath = makeWaveformPath(outputWaveformDisplay);

    g.setColour(Palette::accent);
    g.strokePath(inputPath,
                 juce::PathStrokeType(3.2f,
                                      juce::PathStrokeType::curved,
                                      juce::PathStrokeType::rounded));

    g.setColour(Palette::purple);
    g.strokePath(outputPath,
                 juce::PathStrokeType(1.8f,
                                      juce::PathStrokeType::curved,
                                      juce::PathStrokeType::rounded));

    auto legend = waveformGraphBounds;
    legend = legend.removeFromTop(18).removeFromRight(154);
    const auto drawLegendItem = [&g](juce::Rectangle<int> item,
                                     juce::Colour colour,
                                     const juce::String& label)
    {
        g.setColour(colour);
        g.drawLine(static_cast<float>(item.getX()),
                   static_cast<float>(item.getCentreY()),
                   static_cast<float>(item.getX() + 18),
                   static_cast<float>(item.getCentreY()),
                   2.5f);
        g.setColour(Palette::ink.withAlpha(0.75f));
        g.setFont(juce::Font(juce::FontOptions(11.0f, juce::Font::bold)));
        g.drawText(label, item.withTrimmedLeft(23), juce::Justification::centredLeft, false);
    };

    drawLegendItem(legend.removeFromLeft(72), Palette::accent, "INPUT");
    drawLegendItem(legend, Palette::purple, "OUTPUT");
}

void ReHarmonizerAudioProcessorEditor::drawEnvelopeDisplay(juce::Graphics& g) const
{
    if (envelopeGraphBounds.isEmpty())
        return;

    const float attackMs = audioProcessor.apvts
                               .getRawParameterValue(ReHarmonizerAudioProcessor::paramAttack)
                               ->load();
    const float releaseMs = audioProcessor.apvts
                                .getRawParameterValue(ReHarmonizerAudioProcessor::paramRelease)
                                ->load();
    const float attack = 0.36f * std::sqrt(juce::jlimit(0.0f, 1.0f, attackMs / 5000.0f));
    const float release = 0.40f * std::sqrt(juce::jlimit(0.0f, 1.0f, releaseMs / 5000.0f));

    const auto graph = envelopeGraphBounds.toFloat();
    const float top = graph.getY() + 7.0f;
    const float bottom = graph.getBottom() - 7.0f;
    const float attackX = graph.getX() + graph.getWidth() * attack;
    const float releaseX = graph.getRight() - graph.getWidth() * release;

    g.setColour(Palette::ink.withAlpha(0.09f));
    for (int line = 1; line < 4; ++line)
    {
        const float y = graph.getY() + graph.getHeight() * static_cast<float>(line) / 4.0f;
        g.drawHorizontalLine(static_cast<int>(y), graph.getX(), graph.getRight());
    }

    juce::Path envelope;
    envelope.startNewSubPath(graph.getX(), bottom);
    envelope.lineTo(attackX, top);
    envelope.lineTo(releaseX, top);
    envelope.lineTo(graph.getRight(), bottom);

    juce::Path fill = envelope;
    fill.lineTo(graph.getRight(), bottom);
    fill.lineTo(graph.getX(), bottom);
    fill.closeSubPath();
    g.setColour(Palette::purple.withAlpha(0.13f));
    g.fillPath(fill);

    g.setColour(Palette::purple);
    g.strokePath(envelope,
                 juce::PathStrokeType(2.6f,
                                      juce::PathStrokeType::curved,
                                      juce::PathStrokeType::rounded));

    g.setColour(Palette::accent);
    g.fillEllipse(attackX - 3.5f, top - 3.5f, 7.0f, 7.0f);
    g.fillEllipse(releaseX - 3.5f, top - 3.5f, 7.0f, 7.0f);
}
