/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "LookAndFeel.h"

//==============================================================================
SimpleEqAudioProcessorEditor::SimpleEqAudioProcessorEditor(SimpleEQAudioProcessor& p)
    : AudioProcessorEditor(&p)
    , audioProcessor(p)
    //Sliders
    , peakFreqSlider(*audioProcessor.apvts.getParameter("Peak Freq"), "Hz")
    , peakGainSlider(*audioProcessor.apvts.getParameter("Peak Gain"), "dB")
    , peakQualitySlider(*audioProcessor.apvts.getParameter("Peak Quality"), "")
    , LowCutFreqSlider(*audioProcessor.apvts.getParameter("LowCut Freq"), "Hz")
    , HighCutFreqSlider(*audioProcessor.apvts.getParameter("HighCut Freq"), "Hz")
    , LowCutSlopeSlider(*audioProcessor.apvts.getParameter("LowCut Slope"), "dB/Oct")
    , HighCutSlopeSlider(*audioProcessor.apvts.getParameter("HighCut Slope"), "dB/Oct")
    //Attachments
    , peakFreqSliderAttachment(audioProcessor.apvts, "Peak Freq", peakFreqSlider)
    , peakGainSliderAttachment(audioProcessor.apvts, "Peak Gain", peakGainSlider)
    , peakQualitySliderAttachment(audioProcessor.apvts, "Peak Quality", peakQualitySlider)
    , LowCutFreqSliderAttachment(audioProcessor.apvts, "LowCut Freq", LowCutFreqSlider)
    , highCutFreqSliderAttachment(audioProcessor.apvts, "HighCut Freq", HighCutFreqSlider)
    , LowCutSlopeSliderAttachment(audioProcessor.apvts, "LowCut Slope", LowCutSlopeSlider)
    , HighCutSlopeSliderAttachment(audioProcessor.apvts, "HighCut Slope", HighCutSlopeSlider),
    responseCurveComponent(audioProcessor)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    addLabels();
    for (auto& comp: getComps())
    {
        addAndMakeVisible(comp);
    }
    const auto& params = audioProcessor.getParameters();
    setSize(600, 480);
}
void SimpleEqAudioProcessorEditor::addLabels()
{
    peakFreqSlider.labels.add({0.f, "20Hz"});
    peakFreqSlider.labels.add({1.f, "20KHz"});
    peakGainSlider.labels.add({0.f, "-24dB"});
    peakGainSlider.labels.add({1.f, "+24dB"});
    peakQualitySlider.labels.add({0.f, "0.1"});
    peakQualitySlider.labels.add({1.f, "10.0"});
    LowCutFreqSlider.labels.add({0.f, "20Hz"});
    LowCutFreqSlider.labels.add({1.f, "20KHz"});
    HighCutFreqSlider.labels.add({0.f, "20Hz"});
    HighCutFreqSlider.labels.add({1.f, "20KHz"});
    LowCutSlopeSlider.labels.add({0.f, "20Hz"});
    LowCutSlopeSlider.labels.add({1.f, "20KHz"});
    HighCutSlopeSlider.labels.add({0.f, "20Hz"});
    HighCutSlopeSlider.labels.add({1.f, "20KHz"});
}

SimpleEqAudioProcessorEditor::~SimpleEqAudioProcessorEditor()
{
}

//==============================================================================
void SimpleEqAudioProcessorEditor::paint(juce::Graphics& g)
{
    using namespace juce;
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(Colours::black);
}

void SimpleEqAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    auto bounds = getLocalBounds();
    float hRatio = 25.f/100.f;
    auto responseArea = bounds.removeFromTop(bounds.getHeight() * hRatio);

    responseCurveComponent.setBounds(responseArea);

    bounds.removeFromTop(5);
    auto lowCutArea = bounds.removeFromLeft(bounds.getWidth() * 0.33);
    auto highCutArea = bounds.removeFromRight(bounds.getWidth() * 0.5);

    LowCutFreqSlider.setBounds(lowCutArea.removeFromTop(lowCutArea.getHeight() * 0.5));
    LowCutSlopeSlider.setBounds(lowCutArea);

    HighCutFreqSlider.setBounds(highCutArea.removeFromTop(highCutArea.getHeight() * 0.5));
    HighCutSlopeSlider.setBounds(highCutArea);

    peakFreqSlider.setBounds(bounds.removeFromTop(bounds.getHeight() * 0.33));
    peakGainSlider.setBounds(bounds.removeFromTop((bounds.getHeight() * 0.5)));
    peakQualitySlider.setBounds(bounds);
}
std::vector<juce::Component*> SimpleEqAudioProcessorEditor::getComps()
{
    return {
        &peakFreqSlider,
        &peakGainSlider,
        &peakQualitySlider,
        &LowCutFreqSlider,
        &HighCutFreqSlider,
        &LowCutSlopeSlider,
        &HighCutSlopeSlider,
        &responseCurveComponent
    };
}
