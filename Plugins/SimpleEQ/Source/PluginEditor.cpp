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
    , peakFreqSliderAttachment(audioProcessor.apvts, "Peak Freq", peakFreqSlider)
    , peakGainSliderAttachment(audioProcessor.apvts, "Peak Gain", peakGainSlider)
    , peakQualitySliderAttachment(audioProcessor.apvts, "Peak Quality", peakQualitySlider)
    , LowCutFreqSliderAttachment(audioProcessor.apvts, "LowCut Freq", LowCutFreqSlider)
    , highCutFreqSliderAttachment(audioProcessor.apvts, "HighCut Freq", HighCutFreqSlider)
    , LowCutSlopeSliderAttachment(audioProcessor.apvts, "LowCut Slope", LowCutSlopeSlider)
    , HighCutSlopeSliderAttachment(
          audioProcessor.apvts, "HighCut Slope", HighCutSlopeSlider)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    for (auto& comp: getComps())
    {
        addAndMakeVisible(comp);
    }
    const auto& params = audioProcessor.getParameters();
    for (auto param: params)
    {
        param->addListener(this);
    }
    startTimer(60);
    setSize(600, 400);
}

SimpleEqAudioProcessorEditor::~SimpleEqAudioProcessorEditor()
{
    const auto& params = audioProcessor.getParameters();
    for (auto param: params)
    {
        param->removeListener(this);
    }
}

//==============================================================================
void SimpleEqAudioProcessorEditor::paint(juce::Graphics& g)
{
    using namespace juce;
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(Colours::black);
    auto bounds = getLocalBounds();
    auto responseArea = bounds.removeFromTop(bounds.getHeight() * 0.33);

    auto w = responseArea.getWidth();

    auto& lowCut = monoChain.get<ChainPositions::LowCut>();
    auto& peak = monoChain.get<ChainPositions::Peak>();
    auto& highCut = monoChain.get<ChainPositions::HighCut>();

    auto sampleRate = audioProcessor.getSampleRate();
    std::vector<double> mags;
    mags.resize(w);
    for (int i = 0; i < w; ++i)
    {
        double magnitude = 1.f;
        auto freq = mapToLog10(double(i) / double(w), 20.0, 20000.0);

        if (!monoChain.isBypassed<ChainPositions::Peak>())
        {
            magnitude *= peak.coefficients->getMagnitudeForFrequency(freq, sampleRate);
        }
        if (!lowCut.isBypassed<0>())
            magnitude *=
                lowCut.get<0>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
        if (!lowCut.isBypassed<1>())
            magnitude *=
                lowCut.get<1>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
        if (!lowCut.isBypassed<2>())
            magnitude *=
                lowCut.get<2>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
        if (!lowCut.isBypassed<3>())
            magnitude *=
                lowCut.get<3>().coefficients->getMagnitudeForFrequency(freq, sampleRate);

        if (!highCut.isBypassed<0>())
            magnitude *=
                highCut.get<0>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
        if (!highCut.isBypassed<1>())
            magnitude *=
                highCut.get<1>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
        if (!highCut.isBypassed<2>())
            magnitude *=
                highCut.get<2>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
        if (!highCut.isBypassed<3>())
            magnitude *=
                highCut.get<3>().coefficients->getMagnitudeForFrequency(freq, sampleRate);

        mags[i] = Decibels::gainToDecibels(magnitude);
    }

    Path responseCurve;
    const double outputMin = responseArea.getBottom();
    const double outputMax = responseArea.getY();
    auto map = [outputMin, outputMax](double input)
    { return jmap(input, -24.0, 24.0, outputMin, outputMax); };

    responseCurve.startNewSubPath(responseArea.getX(), map(mags.front()));
    for (size_t i = 1; i < mags.size(); ++i)
    {
        responseCurve.lineTo(responseArea.getX() + i, map(mags[i]));
    }

    g.setColour(Colours::orange);
    g.drawRoundedRectangle(responseArea.toFloat(), 4.f, 1.f);
    g.setColour(Colours::white);
    g.strokePath(responseCurve, PathStrokeType(2.f));
}

void SimpleEqAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    auto bounds = getLocalBounds();
    auto responseArea = bounds.removeFromTop(bounds.getHeight() * 0.33);

    auto lowCutArea = bounds.removeFromLeft(bounds.getWidth() * 0.33);
    auto highCutArea = bounds.removeFromRight(bounds.getWidth() * 0.5);

    LowCutFreqSlider.setBounds(lowCutArea.removeFromTop(lowCutArea.getHeight() * 0.5));
    LowCutSlopeSlider.setBounds(lowCutArea);

    HighCutFreqSlider.setBounds(highCutArea.removeFromTop(highCutArea.getHeight() * 0.5));
    HighCutSlopeSlider.setBounds(highCutArea);

    peakFreqSlider.setBounds(bounds.removeFromTop(bounds.getHeight() * 0.33));
    peakGainSlider.setBounds(bounds.removeFromTop(bounds.getHeight() * 0.5));
    peakQualitySlider.setBounds(bounds);
}
std::vector<juce::Component*> SimpleEqAudioProcessorEditor::getComps()
{
    return {&peakFreqSlider,
            &peakGainSlider,
            &peakQualitySlider,
            &LowCutFreqSlider,
            &HighCutFreqSlider,
            &LowCutSlopeSlider,
            &HighCutSlopeSlider};
}
void SimpleEqAudioProcessorEditor::parameterValueChanged(int parameterIndex,
                                                         float newValue)
{
    parametersChanged.set(true);
}
void SimpleEqAudioProcessorEditor::timerCallback()
{
    if (parametersChanged.compareAndSetBool(false, true))
    {
        //        update monochain
        auto chainSettings = getChainSettings(audioProcessor.apvts);
        auto peakCoeffs = makePeakFilter(chainSettings, audioProcessor.getSampleRate());
        updateCoefficients(monoChain.get<ChainPositions::Peak>().coefficients,
                           peakCoeffs);
        auto lowCutCoeffs =
            makeLowCutFilter(chainSettings, audioProcessor.getSampleRate());
        auto highCutCoeffs =
            makeHighCutFilter(chainSettings, audioProcessor.getSampleRate());

        updateCutFilter(monoChain.get<ChainPositions::LowCut>(),
                        lowCutCoeffs,
                        static_cast<const Slope>(chainSettings.lowCutSlope));
        updateCutFilter(monoChain.get<ChainPositions::HighCut>(),
                        highCutCoeffs,
                        static_cast<const Slope>(chainSettings.highCutSlope));
        //        signal repaint
        repaint();
    }
}
