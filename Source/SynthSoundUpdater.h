/*
  ==============================================================================

    EnvelopeProcessor.h
    Created: 6 Jun 2023 6:31:02pm
    Author:  Marvin

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"



class SynthSoundUpdater : public juce::Thread
{
public:
    SynthSoundUpdater() : juce::Thread("update sound")
    {
        formatManger.registerBasicFormats();
    }
    ~SynthSoundUpdater()
    {
        formatReader = nullptr;
    }


    void run() override
    {
        formatReader.reset(formatManger.createReaderFor(juce::File(path)));

        juce::BigInteger range;
        range.setRange(0, 128, true);

        //s.setEnvelopeParameters(*envelopeParaneters.get());
        sampler->clearSounds();
        sampler->addSound(new juce::SamplerSound("Sample", *formatReader, range, 60, 0.01, 0.1, 16.0));

        formatReader.reset(nullptr);

        sampler.release();
    }

    void updateSound(juce::Synthesiser& s, juce::ADSR::Parameters ep, juce::String p)
    {

        sampler.reset(&s);
        envelopeParaneters.reset(&ep);
        path = p;
        startThread(Priority::highest);
    }


private:

    
    juce::String path; // just for now

    std::unique_ptr<juce::Synthesiser> sampler;
    std::unique_ptr< juce::ADSR::Parameters> envelopeParaneters;

    juce::AudioFormatManager formatManger;

    std::unique_ptr<juce::AudioFormatReader> formatReader { nullptr };

};