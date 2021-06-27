/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "PluginProcessor.h"

struct CustomRotarySlider : juce::Slider {
    CustomRotarySlider() : juce::Slider(juce::Slider::SliderStyle::RotaryVerticalDrag,
                                        juce::Slider::TextEntryBoxPosition::NoTextBox)
    {

    }
};

//==============================================================================
/**
*/
class SimpleEqAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    SimpleEqAudioProcessorEditor(SimpleEQAudioProcessor&);
    ~SimpleEqAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    SimpleEQAudioProcessor& audioProcessor;

    CustomRotarySlider peakFreqSlider;
    CustomRotarySlider peakGainSlider;
    CustomRotarySlider peakQualitySlider;
    CustomRotarySlider LowCutFreqSlider;
    CustomRotarySlider HighCutFreqSlider;
    CustomRotarySlider LowCutSlopeSlider;
    CustomRotarySlider HighCutSlopeSlider;

    using APVTS = juce::AudioProcessorValueTreeState;
    using Attachment = APVTS::SliderAttachment;

    Attachment peakFreqSliderAttachment;
    Attachment peakGainSliderAttachment;
    Attachment peakQualitySliderAttachment;
    Attachment LowCutFreqSliderAttachment;
    Attachment highCutFreqSliderAttachment;
    Attachment LowCutSlopeSliderAttachment;
    Attachment HighCutSlopeSliderAttachment;

    std::vector<juce::Component*> getComps();
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleEqAudioProcessorEditor)
};
