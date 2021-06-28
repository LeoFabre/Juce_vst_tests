/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "PluginProcessor.h"
#include "ResponseCurveComponent.h"

struct LookAndFeel : juce::LookAndFeel_V4
{
    void drawRotarySlider(juce::Graphics& graphics,
                          int x,
                          int y,
                          int width,
                          int height,
                          float sliderPosProportional,
                          float rotaryStartAngle,
                          float rotaryEndAngle,
                          juce::Slider& slider) override;
};

struct RotarySliderWithLabels : juce::Slider
{
    RotarySliderWithLabels(juce::RangedAudioParameter& rap, const juce::String& unitSuffix)
        : juce::Slider(juce::Slider::SliderStyle::RotaryVerticalDrag,
                       juce::Slider::TextEntryBoxPosition::NoTextBox),
        param(&rap),
        suffix(unitSuffix)
    {
        setLookAndFeel(&lnf);
    }
    ~RotarySliderWithLabels() { setLookAndFeel(nullptr); }
    struct LabelPos{
        float pos;
        juce::String label;
    };
    juce::Array<LabelPos> labels;
    void paint(juce::Graphics& graphics) override;
    juce::Rectangle<int> getSliderBounds() const;
    int getTextHeight() const { return 14; }
    juce::String getDisplayString() const;

private:
    juce::RangedAudioParameter* param;
    juce::String suffix;
    LookAndFeel lnf;
};

//==============================================================================
/**
*/
class SimpleEqAudioProcessorEditor
    : public juce::AudioProcessorEditor
{
public:
    SimpleEqAudioProcessorEditor(SimpleEQAudioProcessor&);
    ~SimpleEqAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    SimpleEQAudioProcessor& audioProcessor;

    RotarySliderWithLabels peakFreqSlider;
    RotarySliderWithLabels peakGainSlider;
    RotarySliderWithLabels peakQualitySlider;
    RotarySliderWithLabels LowCutFreqSlider;
    RotarySliderWithLabels HighCutFreqSlider;
    RotarySliderWithLabels LowCutSlopeSlider;
    RotarySliderWithLabels HighCutSlopeSlider;

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

    ResponseCurveComponent responseCurveComponent;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SimpleEqAudioProcessorEditor)
};
