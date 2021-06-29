/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

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
    responseCurveComponent(audioProcessor),
    lowCutBybassButtonAttachment(audioProcessor.apvts, "LowCut Bypassed", lowCutBybassButton),
    peakBypassButtonAttachment(audioProcessor.apvts, "Peak Bypassed", peakBypassButton),
    highCutBybassButtonAttachment(audioProcessor.apvts, "HighCut Bypassed", highCutBybassButton),
    analyzerEnabledButtonAttachment(audioProcessor.apvts, "Analyzer Enabled", analyzerEnabledButton)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    addLabels();
    for (auto& comp: getComps())
    {
        addAndMakeVisible(comp);
    }
    peakBypassButton.setLookAndFeel(&lnf);
    lowCutBybassButton.setLookAndFeel(&lnf);
    highCutBybassButton.setLookAndFeel(&lnf);
    analyzerEnabledButton.setLookAndFeel(&lnf);
    auto safePtr = juce::Component::SafePointer<SimpleEqAudioProcessorEditor>(this);
    analyzerEnabledButton.onClick = [safePtr](){
        if (auto *comp = safePtr.getComponent()) {
            auto enabled = comp->analyzerEnabledButton.getToggleState();
            comp->responseCurveComponent.shouldDraw(enabled);
        }
    };
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
    peakBypassButton.setLookAndFeel(nullptr);
    lowCutBybassButton.setLookAndFeel(nullptr);
    highCutBybassButton.setLookAndFeel(nullptr);
    analyzerEnabledButton.setLookAndFeel(nullptr);
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

    auto analyzerEnabledArea = bounds.removeFromTop(25);
    analyzerEnabledArea.setWidth(100);
    analyzerEnabledArea.setX(5);
    analyzerEnabledArea.removeFromTop(2);
    analyzerEnabledButton.setBounds(analyzerEnabledArea.toNearestInt());
    bounds.removeFromTop(5);

    float hRatio = 25.f/100.f;
    auto responseArea = bounds.removeFromTop(bounds.proportionOfHeight(hRatio));

    responseCurveComponent.setBounds(responseArea);

    bounds.removeFromTop(5);
    auto lowCutArea = bounds.removeFromLeft(bounds.proportionOfWidth(0.33));
    auto highCutArea = bounds.removeFromRight(bounds.proportionOfWidth(0.5));


    lowCutBybassButton.setBounds(lowCutArea.removeFromTop(25).toNearestInt());
    LowCutFreqSlider.setBounds(lowCutArea.removeFromTop(lowCutArea.proportionOfHeight(0.5)));
    LowCutSlopeSlider.setBounds(lowCutArea.toNearestInt());

    highCutBybassButton.setBounds(highCutArea.removeFromTop(25).toNearestInt());
    HighCutFreqSlider.setBounds(highCutArea.removeFromTop(highCutArea.proportionOfHeight(0.5)));
    HighCutSlopeSlider.setBounds(highCutArea.toNearestInt());

    peakBypassButton.setBounds(bounds.removeFromTop(25));
    peakFreqSlider.setBounds(bounds.removeFromTop(bounds.proportionOfHeight(0.33)));
    peakGainSlider.setBounds(bounds.removeFromTop(bounds.proportionOfHeight(0.5)));
    peakQualitySlider.setBounds(bounds.toNearestInt());
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
        &responseCurveComponent,

        &lowCutBybassButton,
        &highCutBybassButton,
        &peakBypassButton,
        &analyzerEnabledButton
    };
}
