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

    
#pragma region Parameters
  
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    juce::AudioProcessorValueTreeState params {*this, nullptr,
        "Parameters", createParameterLayout()};

#pragma endregion

    void loadSample(const juce::String& path, juce::AudioBuffer<float>& b, juce::Synthesiser& s);
    void loadSampleTwo(const juce::String& path);

    //juce::AudioBuffer<float>& getMySampleBufferOne() { return fileBufferOne; }
    juce::AudioBuffer<float>& getMySampleBufferTwo() { return fileBufferTwo; }

    juce::Synthesiser& getSynthOne() { return mySamplerOne; }
    juce::Synthesiser& getSynthTwo() { return mySamplerTwo; }

    float getRmsValueLeft() { return rmsLevelLeft.getCurrentValue(); }
    float getRmsValueRight() { return rmsLevelRight.getCurrentValue(); }


private:
    juce::AudioBuffer<float>  processBuffer, fileBufferTwo;
    //juce::dsp::AudioBlock<float> processBuffer;

    juce::Synthesiser mySamplerOne, mySamplerTwo;
    const int numberOfVoices{ 4 };
    

    juce::LinearSmoothedValue<float>rmsLevelLeft, rmsLevelRight;

    juce::AudioFormatManager formatManger;
    std::unique_ptr<juce::AudioFormatReader> formatReader { nullptr };

    using Filter = juce::dsp::LadderFilter<float>;

    //using CutFilter = juce::dsp::ProcessorChain<Filter, Filter, Filter, Filter>;

    using MonoChain = juce::dsp::ProcessorChain<Filter, Filter>;

    MonoChain leftChain, rightChain;

    enum ChainPostions
    {
        LowCut,
        HighCut
    };

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SampleBasedSynthAudioProcessor)
};

struct ChainSettings
{
    float lowCutFreq{ 0 }, lowQ{ 0 }, highCutFreq{ 0 }, highQ{ 0 };
    int lowCutSlope{ 0 }, highCutSlope{ 0 };
};

ChainSettings getChainSettings(juce::AudioProcessorValueTreeState& params);