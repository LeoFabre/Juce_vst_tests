#include "PluginProcessor.h"
#include "PluginEditor.h"

constexpr bool shouldUseGenericEditor = true;

//A little helper to get the parameter ID
juce::String getParamID(juce::AudioProcessorParameter* param)
{
    if (auto paramWithID = dynamic_cast<juce::AudioProcessorParameterWithID*>(param))
        return paramWithID->paramID;

    return param->getName(50);
}

SimpleEQAudioProcessor::SimpleEQAudioProcessor()
    : juce::AudioProcessor(getBuses())
{
}

void SimpleEQAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                                   juce::MidiBuffer& /*midiMessages*/)

{
}

juce::AudioProcessorEditor* SimpleEQAudioProcessor::createEditor()
{
    if (shouldUseGenericEditor)
        return new juce::GenericAudioProcessorEditor(*this);
    else
        return new SimpleEQProcessorEditor(*this);
}

void SimpleEQAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    //Serializes your parameters, and any other potential data into an XML:

    juce::ValueTree params("Params");

    for (auto& param: getParameters())
    {
        juce::ValueTree paramTree(getParamID(param));
        paramTree.setProperty("Value", param->getValue(), nullptr);
        params.appendChild(paramTree, nullptr);
    }

    juce::ValueTree pluginPreset("MyPlugin");
    pluginPreset.appendChild(params, nullptr);
    //This a good place to add any non-parameters to your preset

    copyXmlToBinary(*pluginPreset.createXml(), destData);
}

void SimpleEQAudioProcessor::setStateInformation(const void* data,
                                                          int sizeInBytes)
{
    //Loads your parameters, and any other potential data from an XML:

    auto xml = getXmlFromBinary(data, sizeInBytes);

    if (xml != nullptr)
    {
        auto preset = juce::ValueTree::fromXml(*xml);
        auto params = preset.getChildWithName("Params");

        for (auto& param: getParameters())
        {
            auto paramTree = params.getChildWithName(getParamID(param));

            if (paramTree.isValid())
                param->setValueNotifyingHost(paramTree["Value"]);
        }

        //Load your non-parameter data now
    }
}

juce::AudioProcessor::BusesProperties SimpleEQAudioProcessor::getBuses()
{
    const auto stereo = juce::AudioChannelSet::stereo();

    return BusesProperties()
        .withInput("Input", stereo, true)
        .withOutput("Output", stereo, true);
}

juce::AudioProcessorValueTreeState::ParameterLayout
    SimpleEQAudioProcessor::CreateParamLayout()
{
    //Spec : 3 bands
    //            Low, High, Parametric/peak
    //            cut band : freq, slope
    //            param band : freq, gain, Q
    juce::AudioProcessorValueTreeState::ParameterLayout res;
    res.add(std::make_unique<juce::AudioParameterFloat>(
        "LowCut Freq",
        "LowCut Freq",
        juce::NormalisableRange<float>
            (20.f, 20000.f, 1.f, 1.f),
        20.f));
    res.add(std::make_unique<juce::AudioParameterFloat>(
        "HighCut Freq",
        "HighCut Freq",
        juce::NormalisableRange<float>
            (20.f, 20000.f, 1.f, 1.f),
        20000.f));
    res.add(std::make_unique<juce::AudioParameterFloat>(
        "Peak Freq",
        "Peak Freq",
        juce::NormalisableRange<float>
            (20.f, 20000.f, 1.f, 1.f),
        750.f));
    res.add(std::make_unique<juce::AudioParameterFloat>(
        "Peak Gain",
        "Peak Gain",
        juce::NormalisableRange<float>
            (-24.f, 24.f, .5f, 1.f),
        0.f));
    res.add(std::make_unique<juce::AudioParameterFloat>(
        "Peak Quality",
        "Peak Quality",
        juce::NormalisableRange<float>
            (0.1f, 10.f, .05f, 1.f),
        1.f));

    juce::StringArray strArr;
    for (int i = 0; i < 4; ++i)
    {
        juce::String str;
        str << (12 + i * 12) << "db/Oct";
        strArr.add(str);
    }
    res.add(std::make_unique<juce::AudioParameterChoice>(
        "LowCut Slope",
        "LowCut Slope",
        strArr,
        0
        ));
    res.add(std::make_unique<juce::AudioParameterChoice>(
        "HighCut Slope",
        "HighCut Slope",
        strArr,
        0
        ));

    return res;
}
void SimpleEQAudioProcessor::prepareToPlay(double sampleRate, int blockSize)
{
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = blockSize;
    spec.numChannels = 1;
    spec.sampleRate = sampleRate;
}

ChainSettings getChainSettings(juce::AudioProcessorValueTreeState& apvts)
{
    ChainSettings res;

    res.lowCutFreq = apvts.getRawParameterValue("LowCut Freq")->load();
    res.highCutFreq = apvts.getRawParameterValue("HighCut Freq")->load();
    res.peakFreq = apvts.getRawParameterValue("Peak Freq")->load();
    res.peakGainDecibels = apvts.getRawParameterValue("Peak Gain")->load();
    res.peakQuality = apvts.getRawParameterValue("Peak Quality")->load();
    res.lowCutSlope = apvts.getRawParameterValue("LowCut Slope")->load();
    res.highCutSlope = apvts.getRawParameterValue("HighCut Slope")->load();
    return res;
}
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SimpleEQAudioProcessor();
}
