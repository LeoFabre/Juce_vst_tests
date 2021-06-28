//
// Created by leofa on 28/06/2021.
//

#ifndef LOOKANDFEEL_H
#define LOOKANDFEEL_H

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
#endif //LOOKANDFEEL_H
