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



class SampleLoader : public juce::Thread, public juce::ActionBroadcaster
{
public:
    SampleLoader() : juce::Thread("Load Sample")
    {
        formatManger.registerBasicFormats();
    }
    ~SampleLoader()
    {
        formatReader = nullptr;
        formatWriter = nullptr;
    }
    

    void run() override
    {
        formatReader.reset(formatManger.createReaderFor(file));

        auto sampleLength = static_cast<int>(formatReader->lengthInSamples);

        buffer->setSize(2, sampleLength);
        formatReader->read(buffer.get(), 0, sampleLength, 0, true, true);
       
        //TO-DO pre-processing

        file = ("D:/test.wav");
        file.deleteFile();

        formatWriter.reset(wavFormat.createWriterFor(new juce::FileOutputStream(file),
            44100,
            buffer->getNumChannels(),
            24,
            {},
            0));

        if (formatWriter != nullptr)
            formatWriter->writeFromAudioSampleBuffer(*buffer.get(), 0, buffer->getNumSamples());

        formatWriter.reset(nullptr);

        juce::BigInteger range;
        range.setRange(0, 128, true);

        sendActionMessage("loaded sample");

        
        formatReader.reset(formatManger.createReaderFor(file));

        sampler->clearSounds();
        sampler->addSound(new juce::SamplerSound("Sample", *formatReader, range, 60, 0.01, 0.1, 16.0));

        buffer.release();
        sampler.release();
    }

    void loadSample(juce::String& p, juce::AudioBuffer<float>& b, juce::Synthesiser& s)
    {
        file = juce::File(p);
        buffer.reset(&b);
        sampler.reset(&s);

        startThread(Priority::normal);  
    }


private:


    juce::File file;
    std::unique_ptr<juce::AudioBuffer<float>> buffer;
    std::unique_ptr<juce::Synthesiser> sampler;

    juce::AudioFormatManager formatManger;

    juce::WavAudioFormat wavFormat;

    std::unique_ptr<juce::AudioFormatReader> formatReader { nullptr };

    std::unique_ptr<juce::AudioFormatWriter> formatWriter { nullptr };

};