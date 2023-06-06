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
    processBuffer.setSize(2, samplesPerBlock);

    rmsLevelLeft.reset(sampleRate, 0.5);
    rmsLevelRight.reset(sampleRate, 0.5);

    rmsLevelLeft.setTargetValue(-100);
    rmsLevelRight.setTargetValue(-100);

    juce::dsp::ProcessSpec spec;

    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = 1;
    spec.sampleRate = sampleRate;

    leftChain.prepare(spec);
    rightChain.prepare(spec);

    leftChain.get<ChainPostions::LowCut>().setMode(juce::dsp::LadderFilterMode::LPF12);
    leftChain.get<ChainPostions::HighCut>().setMode(juce::dsp::LadderFilterMode::HPF12);
    rightChain.get<ChainPostions::LowCut>().setMode(juce::dsp::LadderFilterMode::LPF12);
    rightChain.get<ChainPostions::HighCut>().setMode(juce::dsp::LadderFilterMode::HPF12);
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
    { 
        buffer.clear(i, 0, buffer.getNumSamples());
        processBuffer.clear(i, 0, processBuffer.getNumSamples());
    }

    auto chainSettings = getChainSettings(params);

    leftChain.get<ChainPostions::LowCut>().setCutoffFrequencyHz(chainSettings.lowCutFreq);
    leftChain.get<ChainPostions::HighCut>().setCutoffFrequencyHz(chainSettings.highCutFreq);
    leftChain.get<ChainPostions::LowCut>().setResonance(chainSettings.lowQ);
    leftChain.get<ChainPostions::HighCut>().setResonance(chainSettings.highQ);
    
    rightChain.get<ChainPostions::LowCut>().setCutoffFrequencyHz(chainSettings.lowCutFreq);
    rightChain.get<ChainPostions::HighCut>().setCutoffFrequencyHz(chainSettings.highCutFreq);
    rightChain.get<ChainPostions::LowCut>().setResonance(chainSettings.lowQ);
    rightChain.get<ChainPostions::HighCut>().setResonance(chainSettings.highQ);

    
    mySamplerOne.renderNextBlock(processBuffer, midiMessages, 0, processBuffer.getNumSamples());
    mySamplerTwo.renderNextBlock(processBuffer, midiMessages, 0, processBuffer.getNumSamples());

    
    juce::dsp::AudioBlock<float> block(processBuffer);

    auto leftBlock = block.getSingleChannelBlock(0);
    auto rightBlock = block.getSingleChannelBlock(1);

    juce::dsp::ProcessContextReplacing<float> leftContext(leftBlock);
    juce::dsp::ProcessContextReplacing<float> rightContext(rightBlock);

    leftChain.process(leftContext);
    rightChain.process(rightContext);

    buffer.addFrom(0, 0, processBuffer.getReadPointer(0), processBuffer.getNumSamples());
    buffer.addFrom(1, 0, processBuffer.getReadPointer(1), processBuffer.getNumSamples());

    rmsLevelLeft.skip(buffer.getNumSamples());
    rmsLevelRight.skip(buffer.getNumSamples());

    auto value = juce::Decibels::gainToDecibels(buffer.getRMSLevel(0, 0, buffer.getNumSamples()));
    if (value < rmsLevelLeft.getCurrentValue()) { rmsLevelLeft.setTargetValue(value); }
    else { rmsLevelLeft.setCurrentAndTargetValue(value); }

    value = juce::Decibels::gainToDecibels(buffer.getRMSLevel(1, 0, buffer.getNumSamples()));
    if (value < rmsLevelRight.getCurrentValue()) { rmsLevelRight.setTargetValue(value); }
    else { rmsLevelRight.setCurrentAndTargetValue(value); }
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

ChainSettings getChainSettings(juce::AudioProcessorValueTreeState& params)
{
    ChainSettings settings;
    
    settings.lowCutFreq = params.getRawParameterValue("LowPass Freq")->load();
    settings.highCutFreq = params.getRawParameterValue("HighPass Freq")->load();
    settings.lowQ= params.getRawParameterValue("LowQ")->load();
    settings.highQ = params.getRawParameterValue("HighQ")->load();
    settings.lowCutSlope = params.getRawParameterValue("LowCut Slope")->load();
    settings.highCutSlope = params.getRawParameterValue("HighCut Slope")->load();
    
    return settings;
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
    layout.add(std::make_unique<juce::AudioParameterFloat>("LowPass Freq", "LowPass Freq",
                                                            juce::NormalisableRange<float>(20.0f, 20000.0f, 1.0f, 0.25f),
                                                            20000.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>("LowQ", "LowQ",
                                                            juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f, 1.0f),
                                                            0.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>("HighPass Freq", "HighPass Freq",
                                                            juce::NormalisableRange<float>(20.0f, 20000.0f, 1.0f, 0.25f),
                                                            20.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>("HighQ", "HighQ",
                                                            juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f, 1.0f),
                                                            0.0f));

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
                                                            juce::NormalisableRange<float>(0.1f, 7000.0f, 0.01f, 0.5f), 
                                                            0.1f));

    layout.add(std::make_unique<juce::AudioParameterFloat>("Decay", "Decay",
                                                            juce::NormalisableRange<float>(1.0f, 7000.0f, 0.1f, 0.5f),
                                                            1.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>("Sustain", "Sustain",
                                                            juce::NormalisableRange<float>(-48.0f, 0.0f, 0.1, 5.0f),
                                                            -6.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>("Relase", "Relase",
                                                            juce::NormalisableRange<float>(1.0f, 7000.0f, 0.1f, 0.5f),
                                                            1.0f));

    layout.add(std::make_unique<juce::AudioParameterInt>("Curve", "Curve",
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
                                                            juce::NormalisableRange<float>(-48.0f, 6.0f, 0.1f, 5.5f), 
                                                            0.0f));
   
 
    return layout;
}

//LoadSamples
void SampleBasedSynthAudioProcessor::loadSample(const juce::String& path, juce::AudioBuffer<float>& b, juce::Synthesiser& s)
{
    //DBG("SampleLoader: " << juce::Thread::getThreadName(););

    auto file = juce::File(path);
    formatReader.reset(formatManger.createReaderFor(file));

    auto sampleLength = static_cast<int>(formatReader->lengthInSamples);

    b.setSize(2, sampleLength);
    formatReader->read(&b, 0, sampleLength, 0, true, true);
    //pre-processing
    //juce::Thread::sleep(5000);

    file = ("D:/test.wav");
    file.deleteFile();
    juce::WavAudioFormat format;
    std::unique_ptr<juce::AudioFormatWriter> writer;

    writer.reset(format.createWriterFor(new juce::FileOutputStream(file),
        44100,
        b.getNumChannels(),
        24,
        {},
        0));

    if (writer != nullptr)
        writer->writeFromAudioSampleBuffer(b, 0, b.getNumSamples());

    juce::BigInteger range;
    range.setRange(0, 128, true);

    formatReader.reset(formatManger.createReaderFor(file));

    s.clearSounds();
    s.addSound(new juce::SamplerSound("Sample", *formatReader, range, 60, 0.01, 0.1, 16.0));
}

void SampleBasedSynthAudioProcessor::loadSampleTwo(const juce::String& path)
{
    
    auto file = juce::File(path);
    formatReader.reset(formatManger.createReaderFor(file));
    auto sampleLength = static_cast<int>(formatReader->lengthInSamples);

    fileBufferTwo.setSize(2, sampleLength);
    formatReader->read(&fileBufferTwo, 0, sampleLength, 0, true, true);

    //TO-DO pre-proccessing

    file = ("D:/test.wav");
    file.deleteFile();

    juce::WavAudioFormat format;
    std::unique_ptr<juce::AudioFormatWriter> writer;

    writer.reset(format.createWriterFor(new juce::FileOutputStream(file),
        44100,
        fileBufferTwo.getNumChannels(),
        24,
        {},
        0));

    if (writer != nullptr)
        writer->writeFromAudioSampleBuffer(fileBufferTwo, 0, fileBufferTwo.getNumSamples());

    juce::BigInteger range;
    range.setRange(0, 128, true);

    std::unique_ptr<juce::AudioFormatReader>formatReader2;
    formatReader2.reset(formatManger.createReaderFor(file));
    //formatReader2->read(&fileBufferTwo, 0, sampleLength, 0, true, true);

    mySamplerTwo.clearSounds();
    mySamplerTwo.addSound(new juce::SamplerSound("Sample", *formatReader, range, 60, 0.01, 0.1, 16.0));
}


