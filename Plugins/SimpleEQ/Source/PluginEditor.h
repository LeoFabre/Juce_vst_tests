/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "PluginProcessor.h"

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
    void paint(juce::Graphics& graphics) override;
    juce::Rectangle<int> getSliderBounds() const;
    int getTextHeight() const { return 14; }
    juce::String getDisplayString() const;

private:
    juce::RangedAudioParameter* param;
    juce::String suffix;
    LookAndFeel lnf;
};

struct ResponseCurveComponent: juce::Component
                                , juce::AudioProcessorParameter::Listener
                                , juce::Timer
{
    ResponseCurveComponent(SimpleEQAudioProcessor& p);
    ~ResponseCurveComponent();
    void paint(juce::Graphics& g) override;

    void parameterValueChanged(int parameterIndex, float newValue) override;
    void parameterGestureChanged(int parameterIndex, bool gestureIsStarting) override {};
    void timerCallback() override;
    SimpleEQAudioProcessor& audioProcessor;
    juce::Atomic<bool> parametersChanged {false};
    MonoChain monoChain;
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
