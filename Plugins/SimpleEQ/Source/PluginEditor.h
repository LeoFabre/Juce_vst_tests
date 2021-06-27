#pragma once

#include "PluginProcessor.h"

class SimpleEQProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit SimpleEQProcessorEditor(SimpleEQAudioProcessor&);

    void paint(juce::Graphics&) override;
    void resized() override {}
};
