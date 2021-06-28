//
// Created by leofa on 28/06/2021.
//

#ifndef ROTARYSLIDERWITHLABELS_H
#define ROTARYSLIDERWITHLABELS_H
#include "LookAndFeel.h"
#include "PluginEditor.h"
struct RotarySliderWithLabels : juce::Slider
{
    RotarySliderWithLabels(juce::RangedAudioParameter& rap, const juce::String& unitSuffix)
        : Slider(RotaryVerticalDrag,
                       NoTextBox),
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
#include "PluginProcessor.h"
#include "ResponseCurveComponent.h"
#endif //ROTARYSLIDERWITHLABELS_H
