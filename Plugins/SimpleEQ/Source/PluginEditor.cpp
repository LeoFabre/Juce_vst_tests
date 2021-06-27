#include "PluginProcessor.h"
#include "PluginEditor.h"

SimpleEQProcessorEditor::SimpleEQProcessorEditor(
    SimpleEQAudioProcessor& p)
    : AudioProcessorEditor(&p)
{
    setSize(400, 300);
}

void SimpleEQProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

