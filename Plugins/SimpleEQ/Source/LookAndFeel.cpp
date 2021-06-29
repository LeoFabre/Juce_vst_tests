//
// Created by leofa on 28/06/2021.
//

#include "PluginEditor.h"
void LookAndFeel::drawRotarySlider(juce::Graphics& g,
                                   int x,
                                   int y,
                                   int width,
                                   int height,
                                   float sliderPosProportional,
                                   float rotaryStartAngle,
                                   float rotaryEndAngle,
                                   juce::Slider& slider)
{
    using namespace juce;
    auto bounds = Rectangle<float>(x, y, width, height);
    //purp circle
    g.setColour(Colour(97u, 18u, 167u));
    g.fillEllipse(bounds);

    //circle outline
    g.setColour(Colour(255u, 154u, 1u));
    g.drawEllipse(bounds, 1.f);

    if (auto* rswl = dynamic_cast<RotarySliderWithLabels*>(&slider)) {
        auto center = bounds.getCentre();
        Path p;
        Rectangle<float> r;
        r.setLeft(center.getX() - 2);
        r.setRight(center.getX() + 2);
        r.setTop(bounds.getY());
        r.setBottom(center.getY() - rswl->getTextHeight() * 1.5);
        p.addRoundedRectangle(r, 2.f);
        jassert(rotaryStartAngle < rotaryEndAngle);
        auto sliderAngRad =
            jmap(sliderPosProportional,
                0.f,
                1.f,
                rotaryStartAngle,
                rotaryEndAngle);
        p.applyTransform(
            AffineTransform().rotated(sliderAngRad, center.getX(), center.getY()));
        g.fillPath(p);
        g.setFont(rswl->getTextHeight());
        auto text = rswl->getDisplayString();
        auto strWidth = g.getCurrentFont().getStringWidth(text);
        r.setSize(strWidth + 4, rswl->getTextHeight() + 2);
        r.setCentre(bounds.getCentre());
        g.setColour(Colours::black);
        g.fillRect(r);
        g.setColour(Colours::white);
        g.drawFittedText(text, r.toNearestInt(), Justification::centred, 1);
    }
}
void LookAndFeel::drawToggleButton(juce::Graphics& g,
                                   juce::ToggleButton& button,
                                   bool shouldDrawButtonAsHighlighted,
                                   bool shouldDrawButtonAsDown)
{
    using namespace juce;
    if (dynamic_cast<PowerButton*>(&button)) {
        drawPowerButton(g, button);
    }
    else if (dynamic_cast<AnalyzerButton*>(&button))
    {
        drawAnalButton(g, button);
    }
}
void LookAndFeel::drawPowerButton(juce::Graphics& g,
                                  const juce::ToggleButton& toggleButton)
{
    juce::Path powerButton;
    auto bounds = toggleButton.getLocalBounds();
    auto size = juce::jmin(bounds.toFloat().getWidth(), bounds.toFloat().getHeight() - 6);
    auto r = bounds.toFloat().withSizeKeepingCentre(size, size).toFloat();
    float ang = 30.f;
    size -= 6;
    powerButton.addCentredArc(r.getCentreX(),
                              r.getCentreY(),
                              size * 0.5f,
                              size * 0.5f,
                              0.f,
                              juce::degreesToRadians(ang),
                              juce::degreesToRadians(360.f - ang),
                              true);
    powerButton.startNewSubPath(r.getCentreX(), r.getY());
    powerButton.lineTo(r.getCentre());
    juce::PathStrokeType pathStrokeType(2.f, juce::PathStrokeType::curved);
    auto color = toggleButton.getToggleState() ? juce::Colours::dimgrey : juce::Colour(0u, 172u, 1u);
    g.setColour(color);
    g.strokePath(powerButton, pathStrokeType);
    g.drawEllipse(r, 2);
}
void LookAndFeel::drawAnalButton(juce::Graphics& g, juce::ToggleButton& button)
{
    using namespace juce;
    auto color = !button.getToggleState() ? juce::Colours::dimgrey : juce::Colour(0u, 172u, 1u);
    g.setColour(color);
    auto bounds = button.getLocalBounds();
    g.drawRect(bounds);
    auto insetRect = bounds.reduced(4).toFloat();
    Path randomPath;
    Random random;
    auto startY = insetRect.getY() + insetRect.getHeight();
    randomPath.startNewSubPath(
        insetRect.getX(), startY * random.nextFloat());
    for (int x = 0; x <= insetRect.getRight(); x+=2) {
        randomPath.lineTo(x, startY * random.nextFloat());
    }
    g.strokePath(randomPath, PathStrokeType(1.f));
}
