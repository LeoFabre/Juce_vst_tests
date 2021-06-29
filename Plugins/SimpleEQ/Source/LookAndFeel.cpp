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
                                   juce::ToggleButton& toggleButton,
                                   bool shouldDrawButtonAsHighlighted,
                                   bool shouldDrawButtonAsDown)
{
    using namespace juce;
    Path powerButton;
    auto bounds = toggleButton.getLocalBounds();
    auto size = jmin(bounds.getWidth(), bounds.getHeight() - 6);
    auto r = bounds.withSizeKeepingCentre(size, size).toFloat();
    float ang = 30.f;
    size -= 6;
    powerButton.addCentredArc(r.getCentreX(),
                              r.getCentreY(),
                              size * 0.5,
                              size * 0.5,
                              0.f,
                              degreesToRadians(ang),
                              degreesToRadians(360.f - ang),
                              true);
    powerButton.startNewSubPath(r.getCentreX(), r.getY());
    powerButton.lineTo(r.getCentre());
    PathStrokeType pathStrokeType(2.f, PathStrokeType::curved);
    auto color = toggleButton.getToggleState() ? Colours::dimgrey : Colour(0u, 172u, 1u);
    g.setColour(color);
    g.strokePath(powerButton, pathStrokeType);
    g.drawEllipse(r, 2);
}
