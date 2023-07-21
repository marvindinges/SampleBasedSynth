/*
  ==============================================================================

    PitchDetector.h
    Created: 13 Jun 2023 11:22:08pm
    Author:  Marvin

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class PitchDetector {
public:
    PitchDetector(int bufferSize) : fftOrder(floor(log2(bufferSize))), context(fftOrder) {}

    float detectFrequency(const juce::AudioBuffer<float>& audioBuffer) {
        int numChannels = audioBuffer.getNumChannels();
        int numSamples = audioBuffer.getNumSamples();

        juce::AudioBuffer<float> monoBuffer(1, numSamples);

        for (int channel = 0; channel < numChannels; ++channel) {
            for (int sample = 0; sample < numSamples; ++sample) {
                monoBuffer.addSample(0, sample, audioBuffer.getSample(channel, sample));
            }
        }

        for (int sample = 0; sample < numSamples; ++sample) {
            monoBuffer.setSample(0, sample, monoBuffer.getSample(0, sample) / numChannels);
        }

        juce::FloatVectorOperations::multiply(monoBuffer.getWritePointer(0), window.get(), numSamples);
        context.performFrequencyOnlyForwardTransform(monoBuffer.getWritePointer(0));

        float maxValue = 0.0;
        int maxIndex = 0;

        for (int i = 1; i < numSamples / 2; ++i) {
            float value = monoBuffer.getSample(0, i);

            if (value > maxValue) {
                maxValue = value;
                maxIndex = i;
            }
        }

        float frequency = (float)maxIndex * (float)juce::MathConstants<double>::pi * 2;
        return frequency;
    }

private:
    int fftOrder;
    juce::dsp::FFT context;
    std::unique_ptr<float[]> window;
};