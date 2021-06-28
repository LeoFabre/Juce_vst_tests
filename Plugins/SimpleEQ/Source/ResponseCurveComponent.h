//
// Created by leofa on 28/06/2021.
//

#ifndef RESPONSECURVECOMPONENT_H
#define RESPONSECURVECOMPONENT_H

struct ResponseCurveComponent: juce::Component
                                , juce::AudioProcessorParameter::Listener
                                , juce::Timer
{
    ResponseCurveComponent(SimpleEQAudioProcessor& p);
    ~ResponseCurveComponent();

    void parameterValueChanged(int parameterIndex, float newValue) override;
    void parameterGestureChanged(int parameterIndex, bool gestureIsStarting) override {};
    void timerCallback() override;
    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    SimpleEQAudioProcessor& audioProcessor;
    juce::Atomic<bool> parametersChanged {false};
    MonoChain monoChain;
    void updateChain();
    juce::Image background;
    juce::Rectangle<int> getRenderArea();
    juce::Rectangle<int> getAnalysisArea();
};
#endif //RESPONSECURVECOMPONENT_H
