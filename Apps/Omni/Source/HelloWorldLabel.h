#pragma once

#include "CommonHeader.h"

namespace Omni
{
struct HelloWorldLabel : public juce::Label
{
    HelloWorldLabel()
    {
        setText("HelloWorld", juce::dontSendNotification);
        setJustificationType(juce::Justification::centred);
        setFont(juce::Font(20));
    }
};
} // namespace Omni