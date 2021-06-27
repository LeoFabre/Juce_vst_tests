#pragma once

#include "Parameters.h"
#include "juce_dsp/juce_dsp.h"
struct ChainSettings
{
    float peakFreq {0}, peakGainDecibels {0}, peakQuality {0};
    float lowCutFreq {0}, highCutFreq {0};
    int lowCutSlope {0}, highCutSlope {0};
};

ChainSettings getChainSettings(juce::AudioProcessorValueTreeState& apvts);

class SimpleEQAudioProcessor : public juce::AudioProcessor
{
public:
    SimpleEQAudioProcessor();

    void prepareToPlay(double sampleRate, int blockSize) override;
    void releaseResources() override {}

    bool isBusesLayoutSupported(const BusesLayout&) const override { return true; }

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return JucePlugin_Name; }

    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return juce::String(); }
    void changeProgramName(int, const juce::String& /*newName*/) override {}

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState::ParameterLayout CreateParamLayout();

    juce::AudioProcessorValueTreeState apvts {
        *this,
        nullptr,
        "params",
        CreateParamLayout()
    };

private:
    static BusesProperties getBuses();

};
