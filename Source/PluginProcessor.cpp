/*
                SAMPLE FUSER SYNTH / Fusion Sampler
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SampleBasedSynthAudioProcessor::SampleBasedSynthAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
#pragma region Constructer

    formatManger.registerBasicFormats();

    for (int i = 0; i < numberOfVoices; i++)
    {
        mySamplerOne.addVoice(new juce::SamplerVoice());
        mySamplerTwo.addVoice(new juce::SamplerVoice());
    }

#pragma endregion

}

SampleBasedSynthAudioProcessor::~SampleBasedSynthAudioProcessor()
{
    formatReader = nullptr;
}

//==============================================================================
const juce::String SampleBasedSynthAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SampleBasedSynthAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SampleBasedSynthAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SampleBasedSynthAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double SampleBasedSynthAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SampleBasedSynthAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int SampleBasedSynthAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SampleBasedSynthAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String SampleBasedSynthAudioProcessor::getProgramName (int index)
{
    return {};
}

void SampleBasedSynthAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void SampleBasedSynthAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    mySamplerOne.setCurrentPlaybackSampleRate(sampleRate);
    mySamplerTwo.setCurrentPlaybackSampleRate(sampleRate);
}

void SampleBasedSynthAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SampleBasedSynthAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void SampleBasedSynthAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    //for (int channel = 0; channel < totalNumInputChannels; ++channel)
    //{
    //    auto* channelData = buffer.getWritePointer (channel);
    //
        // ..do something to the data...
    //}

    mySamplerOne.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
    mySamplerTwo.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
}

//==============================================================================
bool SampleBasedSynthAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* SampleBasedSynthAudioProcessor::createEditor()
{
    return new SampleBasedSynthAudioProcessorEditor (*this);
    //return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void SampleBasedSynthAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void SampleBasedSynthAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SampleBasedSynthAudioProcessor();
}

//Parameters
juce::AudioProcessorValueTreeState::ParameterLayout
SampleBasedSynthAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    //Sample & Merge Algorithm Parameters
    layout.add(std::make_unique<juce::AudioParameterFloat>("Gain Sample 1", "Gain Sample 1",
                                                            juce::NormalisableRange<float>(-12.0f, 12.0f, 0.1f, 1.0f), // min, max, interval, skew
                                                            0.0f)); // Default

    layout.add(std::make_unique<juce::AudioParameterFloat>("Gain Sample 2", "Gain Sample 2",
                                                            juce::NormalisableRange<float>(-12.0f, 12.0f, 0.1f, 1.0f), 
                                                            0.0f));

    layout.add(std::make_unique<juce::AudioParameterInt>("Sections", "Sections",
                                                            7,35, 
                                                            21));

    layout.add(std::make_unique<juce::AudioParameterFloat>("Interval", "Interval",
                                                            juce::NormalisableRange<float>(0.05f, 3.0f, 0.05f, 1.0f), 
                                                            0.5f));

    layout.add(std::make_unique<juce::AudioParameterFloat>("Cut", "Cut",
                                                            juce::NormalisableRange<float>(-48.0f, 0.0f, 0.1f, 1.0f), 
                                                            -24.0f));
    //Filter Parameters
    layout.add(std::make_unique<juce::AudioParameterFloat>("LowCut Freq", "LowCut Freq",
                                                            juce::NormalisableRange<float>(20.0f, 20000.0f, 1.0f, 1.0f), 
                                                            20.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>("LowQ", "LowQ",
                                                            juce::NormalisableRange<float>(0.1f, 10.0f, 0.01f, 1.0f), 
                                                            1.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>("HighCut Freq", "HighCut Freq",
                                                            juce::NormalisableRange<float>(20.0f, 20000.0f, 1.0f, 1.0f), 
                                                            20000.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>("HighQ", "HighQ",
                                                            juce::NormalisableRange<float>(0.1f, 10.0f, 0.01f, 1.0f), 
                                                            1.0f));

    juce::StringArray slopeStringArray;
    for (int i = 0; i < 4; i++)
    {
        juce::String str;
        str << (12 + i * 12);
        str << " db/Oct";
        slopeStringArray.add(str);
    }

    layout.add(std::make_unique<juce::AudioParameterChoice>("LowCut Slope", "LowCut Slope", slopeStringArray, 0));
    layout.add(std::make_unique<juce::AudioParameterChoice>("HighCut Slope", "HighCut Slope", slopeStringArray, 0));

    //Envelope Parameters
    layout.add(std::make_unique<juce::AudioParameterFloat>("Attack", "Attack",
                                                            juce::NormalisableRange<float>(0.1f, 7000.0f, 0.01f, 1.0f), 
                                                            0.1f));

    layout.add(std::make_unique<juce::AudioParameterFloat>("Decay", "Decay",
                                                            juce::NormalisableRange<float>(1.0f, 7000.0f, 0.1f, 1.0f), 
                                                            1.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>("Sustain", "Sustain",
                                                            juce::NormalisableRange<float>(-48.0f, 0.0f, 0.1, 1.0f), 
                                                            -6.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>("Relase", "Relase",
                                                            juce::NormalisableRange<float>(1.0f, 7000.0f, 0.1f, 1.0f), 
                                                            1.0f));

    layout.add(std::make_unique<juce::AudioParameterInt>("Slope", "Slope",
                                                            -100, 100, 
                                                            0));
    //Effect Parameters
    layout.add(std::make_unique<juce::AudioParameterFloat>("Dirt", "Dirt",
                                                            juce::NormalisableRange<float>(0.0f, 10.0f, 0.1f, 1.0f), 
                                                            0.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>("Rate", "Rate",
                                                            juce::NormalisableRange<float>(0.05f, 15.0f, 0.01f, 1.0f), 
                                                            1.0f));

    layout.add(std::make_unique<juce::AudioParameterInt>("Depth", "Depth",
                                                            0, 100, 
                                                            50));

    juce::StringArray modType;
    modType.add("Vibrato");
    modType.add("Chorus");
    modType.add("Phaser");
    modType.add("Flanger");

    layout.add(std::make_unique<juce::AudioParameterChoice>("Mod Type", "Mod Type", modType, 0));
    //Modulators (TO DO)
    // 
    //Output Gain
    layout.add(std::make_unique<juce::AudioParameterFloat>("Gain", "Gain",
                                                            juce::NormalisableRange<float>(-48.0f, 6.0f, 0.1f, 1.0f), 
                                                            0.0f));
   

 
    return layout;
}

//LoadSamples
void SampleBasedSynthAudioProcessor::loadSampleOne(const juce::String& path)
{
    mySamplerOne.clearSounds();

    auto file = juce::File(path);
    formatReader.reset(formatManger.createReaderFor(file));

    auto sampleLength = static_cast<int>(formatReader->lengthInSamples);

    sampleBufferOne.setSize(2, sampleLength);
    formatReader->read(&sampleBufferOne, 0, sampleLength, 0, true, true);

    auto bufferLeft = sampleBufferOne.getReadPointer(0);
    auto bufferRight = sampleBufferOne.getReadPointer(1);


    for (int i = 0; i < sampleBufferOne.getNumSamples(); ++i)
    {
        //DBG("Left: " << bufferLeft[i] << " Right: " << bufferRight[i]);
    }
    
    juce::BigInteger range;
    range.setRange(0, 128, true);

    mySamplerOne.addSound(new juce::SamplerSound("Sample", *formatReader, range, 60, 0.01, 0.1, 16.0));
}

void SampleBasedSynthAudioProcessor::loadSampleTwo(const juce::String& path)
{
    mySamplerTwo.clearSounds();

    auto file = juce::File(path);
    formatReader.reset(formatManger.createReaderFor(file));

    auto sampleLength = static_cast<int>(formatReader->lengthInSamples);

    sampleBufferTwo.setSize(2, sampleLength);
    formatReader->read(&sampleBufferTwo, 0, sampleLength, 0, true, true);

    auto bufferLeft = sampleBufferTwo.getReadPointer(0);
    auto bufferRight = sampleBufferTwo.getReadPointer(1);


    juce::BigInteger range;
    range.setRange(0, 128, true);

    mySamplerTwo.addSound(new juce::SamplerSound("Sample", *formatReader, range, 60, 0.01, 0.1, 16.0));
}
