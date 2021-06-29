//
// Created by leofa on 28/06/2021.
//

#include "PluginEditor.h"

ResponseCurveComponent::ResponseCurveComponent(SimpleEQAudioProcessor& p)
    : audioProcessor(p)
    , leftPathProducer(audioProcessor.leftChannelFifo)
    , rightPathProducer(audioProcessor.rightChannelFifo)

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

void ResponseCurveComponent::paint(juce::Graphics& g)
{
    using namespace juce;
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
            magnitude *= peak.coefficients->getMagnitudeForFrequency(freq, sampleRate);

        if (!monoChain.isBypassed<ChainPositions::LowCut>())
        {
            if (!lowCut.isBypassed<0>())
                magnitude *= lowCut.get<0>().coefficients->getMagnitudeForFrequency(
                    freq, sampleRate);
            if (!lowCut.isBypassed<1>())
                magnitude *= lowCut.get<1>().coefficients->getMagnitudeForFrequency(
                    freq, sampleRate);
            if (!lowCut.isBypassed<2>())
                magnitude *= lowCut.get<2>().coefficients->getMagnitudeForFrequency(
                    freq, sampleRate);
            if (!lowCut.isBypassed<3>())
                magnitude *= lowCut.get<3>().coefficients->getMagnitudeForFrequency(
                    freq, sampleRate);
        }

        if (!monoChain.isBypassed<ChainPositions::HighCut>())
        {
            if (!highCut.isBypassed<0>())
                magnitude *= highCut.get<0>().coefficients->getMagnitudeForFrequency(
                    freq, sampleRate);
            if (!highCut.isBypassed<1>())
                magnitude *= highCut.get<1>().coefficients->getMagnitudeForFrequency(
                    freq, sampleRate);
            if (!highCut.isBypassed<2>())
                magnitude *= highCut.get<2>().coefficients->getMagnitudeForFrequency(
                    freq, sampleRate);
            if (!highCut.isBypassed<3>())
                magnitude *= highCut.get<3>().coefficients->getMagnitudeForFrequency(
                    freq, sampleRate);
        }

        mags[i] = Decibels::gainToDecibels(magnitude);
    }
    Path responseCurve;
    const float outputMin = responseArea.toFloat().getBottom();
    const float outputMax = responseArea.toFloat().getY();
    auto map = [outputMin, outputMax](float input)
    { return jmap(input, -24.f, 24.f, outputMin, outputMax); };
    responseCurve.startNewSubPath(responseArea.toFloat().getX(), map((float)mags.front()));
    for (size_t i = 1; i < mags.size(); ++i)
    {
        responseCurve.lineTo(responseArea.toFloat().getX() + (float)i, map((float)mags[i]));
    }

    //    FFT
    responseArea.removeFromRight(15);
    if (_shouldDrawFFT)
    {
        auto leftChannelFFTPath = leftPathProducer.getPath();
        leftChannelFFTPath.applyTransform(AffineTransform::translation(responseArea.getX(), responseArea.getY()));

        g.setColour(Colour(97u, 18u, 167u)); //purple-
        g.strokePath(leftChannelFFTPath, PathStrokeType(1.f));

        auto rightChannelFFTPath = rightPathProducer.getPath();
        rightChannelFFTPath.applyTransform(AffineTransform::translation(responseArea.getX(), responseArea.getY()));

        g.setColour(Colour(215u, 201u, 134u));
        g.strokePath(rightChannelFFTPath, PathStrokeType(1.f));
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
    //map<freq, shouldDrawLabel>
    std::map<float, bool> freqs {{20.f, true},
                                 {30.f, false},
                                 {40.f, false},
                                 {50.f, true},
                                 {100.f, true},
                                 {200.f, true},
                                 {300.f, false},
                                 {400.f, false},
                                 {500.f, true},
                                 {1000.f, true},
                                 {2000.f, true},
                                 {3000.f, false},
                                 {4000.f, false},
                                 {5000.f, true},
                                 {10000.f, true},
                                 {20000.f, true}};
    auto renderArea = getAnalysisArea().toFloat();
    auto left = renderArea.getX();
    auto right = renderArea.getRight();
    auto top = renderArea.getY();
    auto bottom = renderArea.getBottom();
    auto width = renderArea.getWidth();
    Array<float> xs;
    for (auto freq: freqs)
    {
        auto f = freq.first;
        auto normX = mapFromLog10(f, 20.f, 20000.f);
        xs.add(left + width * normX);
    }
    g.setColour(Colours::dimgrey);
    for (auto x: xs)
        g.drawVerticalLine((int)x, top, bottom);
    Array<float> gain {-24.f, -12.f, 0.f, 12.f, 24.f};
    for (const auto& gDb: gain)
    {
        auto y = jmap(gDb, -24.f, 24.f, float(bottom), float(top));
        g.setColour(gDb == 0.f ? Colour(0u, 172u, 1u) : Colours::darkgrey);
        g.drawHorizontalLine((int)y, left, right);
    }
    g.setColour(Colours::lightgrey);
    const int fontHeight = 10;
    g.setFont(fontHeight);
    int i = 0;
    for (const auto& freq: freqs)
    {
        auto f = freq.first;
        auto x = xs[i];
        bool addK = false;
        String str;
        if (f > 999.f)
        {
            addK = true;
            f /= 1000.f;
        }
        str << f;
        if (addK)
            str << "k";
        str << "Hz";
        auto textWidth = g.getCurrentFont().getStringWidth(str);
        Rectangle<int> r;
        r.setSize(textWidth, fontHeight);
        r.setCentre((int)x, 0);
        r.setY(1);
        if (freq.second) //if draw label...
            g.drawFittedText(str, r, Justification::centred, 1);
        i++;
    }

    for (const auto& gDb: gain)
    {
        auto y = jmap(gDb, -24.f, 24.f, float(bottom), float(top));
        String str;
        if (gDb > 0)
            str << "+";
        str << gDb;
        auto textWidth = g.getCurrentFont().getStringWidth(str);
        Rectangle<int> r;
        r.setSize(textWidth, fontHeight);
        r.setX(getWidth() - textWidth);
        r.setCentre(r.getCentreX(), (int)y);
        g.setColour(gDb == 0.f ? Colour(0u, 172u, 1u) : Colours::lightgrey);
        g.drawFittedText(str, r, Justification::centred, 1);

        str.clear();
        str << (gDb - 24.f);
        r.setX(1);
        textWidth = g.getCurrentFont().getStringWidth(str);
        r.setSize(textWidth, fontHeight);
        g.setColour(Colours::lightgrey);
        g.drawFittedText(str, r, Justification::centred, 1);
    }
}

void ResponseCurveComponent::parameterValueChanged(int parameterIndex, float newValue)
{
    parametersChanged.set(true);
}

void PathProducer::process(juce::Rectangle<float> fftBounds, double sampleRate)
{
    juce::AudioBuffer<float> tempIncomingBuffer;
    while (leftChannelFifo->getNumCompleteBuffersAvailable() > 0)
    {
        if (leftChannelFifo->getAudioBuffer(tempIncomingBuffer))
        {
            auto size = tempIncomingBuffer.getNumSamples();
            juce::FloatVectorOperations::copy(monoBuffer.getWritePointer(0, 0),
                                              monoBuffer.getReadPointer(0, size),
                                              monoBuffer.getNumSamples() - size);
            juce::FloatVectorOperations::copy(
                monoBuffer.getWritePointer(0, monoBuffer.getNumSamples() - size),
                tempIncomingBuffer.getReadPointer(0, 0),
                size);
            leftChannelFFTDataGenerator.produceFFTDataForRendering(monoBuffer, -48.f);
        }
    }

    const auto fftSize = leftChannelFFTDataGenerator.getFFTSize();
    const auto binWidth = sampleRate / (double) fftSize;
    while (leftChannelFFTDataGenerator.getNumAvailableFFTDataBlocks() > 0)
    {
        std::vector<float> fftData;
        if (leftChannelFFTDataGenerator.getFFTData(fftData))
            pathProducer.generatePath(fftData, fftBounds, fftSize, (float)binWidth, -48.f);
    }

    while (pathProducer.getNumPathsAvailable())
    {
        pathProducer.getPath(leftChannelFFTPath);
    }
}

void ResponseCurveComponent::timerCallback()
{
    if (_shouldDrawFFT)
    {
        auto fftBounds = getAnalysisArea().toFloat();
        fftBounds.removeFromRight(15);
        auto sampleRate = audioProcessor.getSampleRate();
        leftPathProducer.process(fftBounds, sampleRate);
        rightPathProducer.process(fftBounds, sampleRate);
    }
    if (parametersChanged.compareAndSetBool(false, true))
        updateChain();
    repaint();
}

void ResponseCurveComponent::updateChain()
{
    auto chainSettings = getChainSettings(audioProcessor.apvts);
    monoChain.setBypassed<ChainPositions::LowCut>(chainSettings.lowCutBypassed);
    monoChain.setBypassed<ChainPositions::Peak>(chainSettings.peakBypassed);
    monoChain.setBypassed<ChainPositions::HighCut>(chainSettings.highCutBypassed);
    auto peakCoeffs = makePeakFilter(chainSettings, audioProcessor.getSampleRate());
    auto lowCutCoeffs = makeLowCutFilter(chainSettings, audioProcessor.getSampleRate());
    auto highCutCoeffs = makeHighCutFilter(chainSettings, audioProcessor.getSampleRate());

    updateCoefficients(monoChain.get<Peak>().coefficients, peakCoeffs);
    updateCutFilter(monoChain.get<LowCut>(), lowCutCoeffs, chainSettings.lowCutSlope);
    updateCutFilter(monoChain.get<HighCut>(), highCutCoeffs, chainSettings.highCutSlope);
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
//    bounds.removeFromRight(15);
    return bounds;
}
void ResponseCurveComponent::shouldDraw(bool b)
{
    _shouldDrawFFT = b;
}
