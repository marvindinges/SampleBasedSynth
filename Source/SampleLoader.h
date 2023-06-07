/*
  ==============================================================================

    SampleLoader.h
    Created: 3 Jun 2023 1:31:02pm
    Author:  Marvin

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"



class SampleLoader : public juce::Thread, public juce::ActionBroadcaster, public juce::CriticalSection
{
public:
    SampleLoader() : juce::Thread("loading sample")
    {
        formatManger.registerBasicFormats();
    }
    ~SampleLoader()
    {
        formatReader = nullptr;
    }
    

    void run() override
    {
        formatReader.reset(formatManger.createReaderFor(juce::File(path)));

        auto sampleLength = static_cast<int>(formatReader->lengthInSamples);

        buffer->setSize(2, sampleLength);
        formatReader->read(buffer.get(), 0, sampleLength, 0, true, true);

        buffer.release();
        formatReader.reset(nullptr);

        sendActionMessage(message);
    }

    void loadSample(juce::String& p, juce::AudioBuffer<float>& b, const juce::String& m)
    {
        path = p;
        buffer.reset(&b);
        message = m;
        startThread(Priority::normal);  
    }


private:

    
    juce::String path;
    std::unique_ptr<juce::AudioBuffer<float>> buffer;
    juce::String message;

    juce::AudioFormatManager formatManger;

    std::unique_ptr<juce::AudioFormatReader> formatReader { nullptr };
};