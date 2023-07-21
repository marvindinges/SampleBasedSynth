/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "LevelMeter.h"
#include "SampleLoader.h"
#include "PreProcessor.h"
#include "SynthSoundUpdater.h"
#include "LockHolder.h"
#include "PitchDetector.h"


//==============================================================================
/**
*/
class SampleBasedSynthAudioProcessorEditor : public juce::AudioProcessorEditor,
    public juce::FileDragAndDropTarget,
    public juce::Timer,
    public juce::ActionListener
{
public:
    SampleBasedSynthAudioProcessorEditor (SampleBasedSynthAudioProcessor&);
    ~SampleBasedSynthAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    bool isInterestedInFileDrag(const juce::StringArray& files) override;
    void filesDropped(const juce::StringArray& files, int x, int y) override;

    void timerCallback() override;
    void actionListenerCallback(const juce::String& message) override;


    bool holdingLockOn(juce::CriticalSection &t);

private:
    juce::Rectangle<float> sampleRec, outputRec, filterRec, envelopeRec, modRec, effectRec;
    juce::Rectangle<float> zoneOne, zoneTwo;
    juce::Rectangle<float> sampleOneBackground, sampleTwoBackground;
   
    SampleLoader sampleLoader;
    PreProcessor preProcessor;
    SynthSoundUpdater updater;
    LockHolder lockHolder;

    juce::AudioBuffer<float> fileBufferOne, fileBufferTwo;

    juce::String pathOne, pathTwo; //temp will not be needed later on

    std::vector<float> pointsOneLeft, pointsOneRight;
    std::vector<float> pointsTwoLeft, pointsTwoRight;

    bool loadedSampleOne{ false };
    bool loadedSampleTwo{ false };


    Gui::LevelMeter levelMeterLeft, levelMeterRight;

    juce::Slider lowPassSlider, highPassSlider, lowQ_Slider, highQ_Slider;
    juce::ComboBox lowCutSlopeBox, highCutSlopeBox;

    juce::Slider attackSlider, decaySlider, sustainSlider, relaseSlider, curveSlider;

    juce::Slider masterGainSlider;


    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        lowPassSA, lowQ_SA, highpassSA, highQ_SA, attackSA, decaySA, sustainSA, relaseSA, curveSA, masterGainSA;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment>
        lowCutSlopeCBA, highCutSlopeCBA;

    
    //std::unique_ptr<juce::Thread> runningThread { nullptr };
    //std::unique_ptr<juce::CriticalSection> lockingThread { nullptr };
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    SampleBasedSynthAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SampleBasedSynthAudioProcessorEditor)
};
