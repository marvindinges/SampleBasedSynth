/*
  ==============================================================================

    PreProcessor.h
    Created: 6 Jun 2023 7:00:03pm
    Author:  Marvin

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"



class PreProcessor : public juce::Thread, public juce::ActionBroadcaster, public juce::CriticalSection
{
public:
    PreProcessor() : juce::Thread("apllying envelope")
    {
    }
    ~PreProcessor()
    {
        formatWriter = nullptr;
    }


    void run() override
    {
        //TO-DO pre-processing

        juce::File file("D:/test.wav"); // <- TO-DO path to temp folder
        file.deleteFile();

        //write buffer as file;
        formatWriter.reset(wavFormat.createWriterFor(new juce::FileOutputStream(file),
            44100,
            bufferOne->getNumChannels(),
            24,
            {},
            0));

        if (formatWriter != nullptr)
            formatWriter->writeFromAudioSampleBuffer(*bufferOne.get(), 0, bufferOne->getNumSamples());

        formatWriter.reset(nullptr);

        bufferOne.release();
        bufferTwo.release();
        sendActionMessage("PP");
    }

    void preProcessing(juce::AudioBuffer<float>& b1, juce::AudioBuffer<float>& b2)
    {
        bufferOne.reset(&b1);
        bufferTwo.reset(&b2);

        startThread(Priority::highest);
    }



private:

    std::unique_ptr<juce::AudioBuffer<float>> bufferOne, bufferTwo;

    juce::WavAudioFormat wavFormat;

    std::unique_ptr<juce::AudioFormatWriter> formatWriter { nullptr };


};