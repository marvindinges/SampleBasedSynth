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

    void loadSampleOne(const juce::String& path);
    void loadSampleTwo(const juce::String& path);


    juce::AudioBuffer<float>& getMySampleBufferOne() { return sampleBufferOne; }
    juce::AudioBuffer<float>& getMySampleBufferTwo() { return sampleBufferTwo; }

private:
    juce::AudioBuffer<float> sampleBufferOne;
    juce::AudioBuffer<float> sampleBufferTwo;

    juce::Synthesiser mySamplerOne;
    juce::Synthesiser mySamplerTwo;
    const int numberOfVoices{ 4 };

    juce::AudioFormatManager formatManger;
    std::unique_ptr<juce::AudioFormatReader> formatReader { nullptr };
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SampleBasedSynthAudioProcessor)
};
