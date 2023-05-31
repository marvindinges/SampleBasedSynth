/*
  ==============================================================================

    LevelMeter.h
    Created: 31 May 2023 7:51:18pm
    Author:  Marvin

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

namespace Gui 
{
    class LevelMeter : public juce::Component
    {
    public:
        void paint(juce::Graphics& g) override
        {
            auto bounds = getLocalBounds().toFloat();

            g.setColour(juce::Colours::darkgrey);
            g.fillRoundedRectangle(bounds, 15.0f);

            if (level > -60.0f)
            {
                g.setGradientFill(gradient);
                const auto scaled = juce::jmap(level, -60.0f, 6.0f, 0.0f, static_cast<float>(getHeight()));
                g.fillRoundedRectangle(bounds.removeFromBottom(scaled), 15.0f);
            }
        }
        void resized() override
        {
            auto bounds = getLocalBounds().toFloat();

            gradient = juce::ColourGradient{
                            juce::Colours::lightgreen,
                            bounds.getBottomLeft(),
                            juce::Colours::red.withBrightness(0.5f),
                            bounds.getTopLeft(),
                            false
            };
            gradient.addColour(0.5, juce::Colours::yellow);
        }
        void setLevel(const float value) { level = value; }
    private:
        float level = -60.0f;
        juce::ColourGradient gradient;
    };
}
