//
// Created by leofa on 28/06/2021.
//

#include "PluginEditor.h"
#include "PluginProcessor.h"
#include "ResponseCurveComponent.h"
void ResponseCurveComponent::parameterValueChanged(int parameterIndex,
    float newValue)
{
    parametersChanged.set(true);
}
void ResponseCurveComponent::timerCallback()
{
    if (parametersChanged.compareAndSetBool(false, true))
    {
        updateChain();
        repaint();
    }
}
void ResponseCurveComponent::updateChain()
{
    auto chainSettings = getChainSettings(audioProcessor.apvts);
    auto peakCoeffs = makePeakFilter(chainSettings, audioProcessor.getSampleRate());
    updateCoefficients(monoChain.get<Peak>().coefficients,
        peakCoeffs);
    auto lowCutCoeffs =
        makeLowCutFilter(chainSettings, audioProcessor.getSampleRate());
    auto highCutCoeffs =
        makeHighCutFilter(chainSettings, audioProcessor.getSampleRate());

    updateCutFilter(monoChain.get<LowCut>(),
        lowCutCoeffs,
        static_cast<const Slope>(chainSettings.lowCutSlope));
    updateCutFilter(monoChain.get<HighCut>(),
        highCutCoeffs,
        static_cast<const Slope>(chainSettings.highCutSlope));
}
void ResponseCurveComponent::paint(juce::Graphics& g)
{
    using namespace juce;
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(Colours::black);
    g.drawImage(background, getLocalBounds().toFloat());
    auto responseArea = getAnalysisArea();

    auto w = responseArea.getWidth();

    auto& lowCut = monoChain.get<LowCut>();
    auto& peak = monoChain.get<Peak>();
    auto& highCut = monoChain.get<HighCut>();

    auto sampleRate = audioProcessor.getSampleRate();
    std::vector<double> mags;
    mags.resize(w);
    for (int i = 0; i < w; ++i)
    {
        double magnitude = 1.f;
        auto freq = mapToLog10(double(i) / double(w), 20.0, 20000.0);

        if (!monoChain.isBypassed<Peak>())
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
    g.drawRoundedRectangle(getRenderArea().toFloat(), 4.f, 1.f);
    g.setColour(Colours::white);
    g.strokePath(responseCurve, PathStrokeType(2.f));
}
void ResponseCurveComponent::resized()
{
    using namespace juce;
    background = Image(Image::PixelFormat::RGB, getWidth(), getHeight(), true);
    Graphics g(background);
    Array<float> freqs
    {
        20,30,40,50,100,
        200,300,400,500,1000,
        2000,3000,4000,5000,10000,
        20000
    };
    auto renderArea = getAnalysisArea();
    auto left = renderArea.getX();
    auto right = renderArea.getRight();
    auto top = renderArea.getY();
    auto bottom = renderArea.getBottom();
    auto width = renderArea.getWidth();
    Array<float> xs;
    for (auto f : freqs)
    {
        auto normX = mapFromLog10(f, 20.f, 20000.f);
        xs.add(left + width * normX);
    }
    g.setColour(Colours::dimgrey);
    for (auto x : xs)
    {
        g.drawVerticalLine(x, top, bottom);
    }
    Array<float> gain {-24, -12, 0, 12, 24};
    for (const auto& gDb: gain)
    {
        auto y = jmap(gDb, -24.f, 24.f, float(bottom),float(top));
//        g.drawHorizontalLine(y, 0, getWidth());
        g.setColour(gDb == 0.f ? Colour(0u, 172u, 1u) : Colours::darkgrey);
        g.drawHorizontalLine(y, left, right);
    }
}
juce::Rectangle<int> ResponseCurveComponent::getRenderArea()
{
    auto bounds = getLocalBounds();
//    bounds.reduce(10, 8);
    bounds.removeFromTop(12);
    bounds.removeFromBottom(2);
    bounds.removeFromLeft(20);
    bounds.removeFromRight(20);
    return bounds;
}

juce::Rectangle<int> ResponseCurveComponent::getAnalysisArea()
{
    auto bounds = getRenderArea();
    bounds.removeFromTop(4);
    bounds.removeFromBottom(4);
    return bounds;
}
ResponseCurveComponent::ResponseCurveComponent(SimpleEQAudioProcessor& p)
    :audioProcessor(p)
{
    const auto& params = audioProcessor.getParameters();
    for (auto param: params)
    {
        param->addListener(this);
    }
    updateChain();
    startTimer(10);
}
ResponseCurveComponent::~ResponseCurveComponent()
{
    const auto& params = audioProcessor.getParameters();
    for (auto param: params)
    {
        param->removeListener(this);
    }
}
