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
}

ReHarmonizerAudioProcessorEditor::~ReHarmonizerAudioProcessorEditor()
{

}

void ReHarmonizerAudioProcessorEditor::paint(juce::Graphics&)
{

}

void ReHarmonizerAudioProcessorEditor::resized()
{

}
