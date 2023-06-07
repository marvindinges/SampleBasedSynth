/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
class SampleBasedSynthAudioProcessor  : public juce::AudioProcessor
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    SampleBasedSynthAudioProcessor();
    ~SampleBasedSynthAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;


    juce::Synthesiser& getSynthOne() { return mySamplerOne; }
    juce::Synthesiser& getSynthTwo() { return mySamplerTwo; }

    juce::ADSR::Parameters& getEnvelopeParametesOne() { return envelopeParmetersOne; }
    juce::ADSR::Parameters& getEnvelopeParametesTwo() { return envelopeParmetersTwo; }

    float getRmsValueLeft() { return rmsLevelLeft.getCurrentValue(); }
    float getRmsValueRight() { return rmsLevelRight.getCurrentValue(); }
  
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    juce::AudioProcessorValueTreeState params {*this, nullptr, "Parameters", createParameterLayout()};

private:

    juce::AudioBuffer<float>  processBuffer;

    juce::Synthesiser mySamplerOne, mySamplerTwo;
    const int numberOfVoices{ 4 };

    juce::ADSR::Parameters envelopeParmetersOne;
    juce::ADSR::Parameters envelopeParmetersTwo;

    juce::LinearSmoothedValue<float>rmsLevelLeft, rmsLevelRight;


    using Filter = juce::dsp::LadderFilter<float>;

    using MonoChain = juce::dsp::ProcessorChain<Filter, Filter>;

    MonoChain leftChain, rightChain;

    enum ChainPostions
    {
        LowCut,
        HighCut
    };

    float previousGain;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SampleBasedSynthAudioProcessor)
};

struct ChainSettings
{
    float lowCutFreq{ 0 }, lowQ{ 0 }, highCutFreq{ 0 }, highQ{ 0 }, a{ 0 }, d{ 0 }, s{ 0 }, r{ 0 }, gain{ 0 };
    int lowCutSlope{ 0 }, highCutSlope{ 0 };
};

ChainSettings getChainSettings(juce::AudioProcessorValueTreeState& params);