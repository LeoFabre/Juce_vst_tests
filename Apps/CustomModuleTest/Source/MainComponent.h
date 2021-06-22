#pragma once

#include "CommonHeader.h"

namespace Omni
{
class MainComponent : public Component
{
public:
    MainComponent();

    void paint(Graphics&) override;
    void resized() override;

private:
    CustomModule::DummyLabel dummyLabel;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
} // namespace Omni
