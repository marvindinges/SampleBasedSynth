/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SampleBasedSynthAudioProcessorEditor::SampleBasedSynthAudioProcessorEditor (SampleBasedSynthAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    getLookAndFeel().setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::white.withAlpha(0.0f));
    
    //loadSampleOneButton.onClick = [&]() { audioProcessor.loadSample(); };
    //addAndMakeVisible(loadSampleOneButton);
    // 
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    
    //Sample One
    addAndMakeVisible(sampleOneAsCnv);
    addAndMakeVisible(sampleOneInCnv);

    sampleOneGain.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    sampleOneGain.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 60, 20);
    sampleOneGain.setColour(juce::Slider::thumbColourId, juce::Colours::deepskyblue.brighter(0.6f));
    addAndMakeVisible(sampleOneGain);

    addAndMakeVisible(sampleOneMute);
    addAndMakeVisible(sampleOneReverse);

    //Sample Two
    addAndMakeVisible(sampleTwoAsCnv);
    addAndMakeVisible(sampleTwoInCnv);

    sampleTwoGain.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    sampleTwoGain.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 60, 20);
    sampleTwoGain.setColour(juce::Slider::thumbColourId, juce::Colours::greenyellow.brighter(0.0f));
    addAndMakeVisible(sampleTwoGain);

    addAndMakeVisible(sampleTwoMute);
    addAndMakeVisible(sampleTwoReverse);

    //Fusion Sample
    addAndMakeVisible(fusionSampleAsCnv);
    addAndMakeVisible(fusionSampleInCnv);

    fusionSampleGain.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    fusionSampleGain.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 60, 20);
    fusionSampleGain.setColour(juce::Slider::thumbColourId, juce::Colours::yellow.darker(0.2f));
    addAndMakeVisible(fusionSampleGain);

    addAndMakeVisible(fusionSampleMute);
    addAndMakeVisible(fusionSampleReverse);

    //Noise Section
    addAndMakeVisible(noiseType);

    noiseGainSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    noiseGainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 60, 20);
    noiseGainSlider.setColour(juce::Slider::thumbColourId, juce::Colours::darkgrey.darker(0.0f));
    addAndMakeVisible(noiseGainSlider);

    combFilterSlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    combFilterSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 60, 20);
    combFilterSlider.setColour(juce::Slider::thumbColourId, juce::Colours::darkgrey.darker(0.0f));
    addAndMakeVisible(combFilterSlider);

    addAndMakeVisible(noiseInCnv);
    addAndMakeVisible(noiseMute);


    //Filter Section
    lowPassSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    lowPassSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 60, 20);
    lowPassSlider.setColour(juce::Slider::thumbColourId, juce::Colours::purple.brighter(0.5f));
    addAndMakeVisible(lowPassSlider);
    lowPassSA = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.params, "LowPass Freq", lowPassSlider);

    lowQ_Slider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    lowQ_Slider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 60, 20);
    lowQ_Slider.setColour(juce::Slider::thumbColourId, juce::Colours::purple.brighter(0.5f));
    addAndMakeVisible(lowQ_Slider);
    lowQ_SA = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.params, "LowQ", lowQ_Slider);

    highPassSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    highPassSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 60, 20);
    highPassSlider.setColour(juce::Slider::thumbColourId, juce::Colours::purple.brighter(0.5f));
    addAndMakeVisible(highPassSlider);
    highpassSA = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.params, "HighPass Freq", highPassSlider);

    highQ_Slider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    highQ_Slider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 60, 20);
    highQ_Slider.setColour(juce::Slider::thumbColourId, juce::Colours::purple.brighter(0.5f));
    addAndMakeVisible(highQ_Slider);
    highQ_SA = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.params, "HighQ", highQ_Slider);

    addAndMakeVisible(lowCutSlopeBox);
    lowCutSlopeCBA = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.params, "LowCut Slope", lowCutSlopeBox);
    addAndMakeVisible(highCutSlopeBox);
    highCutSlopeCBA = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.params, "HighCut Slope", highCutSlopeBox);

    //Envelope Section
    attackSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    attackSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 60, 20);
    attackSlider.setColour(juce::Slider::thumbColourId, juce::Colours::hotpink.brighter(0.2f));
    addAndMakeVisible(attackSlider);
    attackSA = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.params, "Attack", attackSlider);
    
    decaySlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    decaySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 60, 20);
    decaySlider.setColour(juce::Slider::thumbColourId, juce::Colours::hotpink.brighter(0.2f));
    addAndMakeVisible(decaySlider);
    decaySA = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.params, "Decay", decaySlider);

    sustainSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    sustainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 60, 20);
    sustainSlider.setColour(juce::Slider::thumbColourId, juce::Colours::hotpink.brighter(0.2f));
    addAndMakeVisible(sustainSlider);
    sustainSA = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.params, "Sustain", sustainSlider);

    relaseSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    relaseSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 60, 20);
    relaseSlider.setColour(juce::Slider::thumbColourId, juce::Colours::hotpink.brighter(0.2f));
    addAndMakeVisible(relaseSlider);
    relaseSA = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.params, "Relase", relaseSlider);

    curveSlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    curveSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    curveSlider.setColour(juce::Slider::thumbColourId, juce::Colours::hotpink.brighter(0.2f));
    curveSlider.setColour(juce::Slider::trackColourId, juce::Colours::white.withAlpha(0.0f));
    addAndMakeVisible(curveSlider);
    curveSA = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.params, "Curve", curveSlider);

    //Output Section
    masterGainSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    masterGainSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    masterGainSlider.setColour(juce::Slider::thumbColourId, juce::Colours::lightgreen);
    addAndMakeVisible(masterGainSlider);
    masterGainSA = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.params, "Gain", masterGainSlider);

    addAndMakeVisible(levelMeterLeft);
    addAndMakeVisible(levelMeterRight);

    
    setSize (1500, 750);
    startTimerHz(24);
    
    sampleLoader.addActionListener(this);
}

SampleBasedSynthAudioProcessorEditor::~SampleBasedSynthAudioProcessorEditor()
{
}

//==============================================================================
void SampleBasedSynthAudioProcessorEditor::paint (juce::Graphics& g)
{

    g.fillAll (juce::Colours::white);

    g.setColour(juce::Colours::slategrey);
    
    g.drawRoundedRectangle(sampleRec, 15, 2);
    g.drawRoundedRectangle(outputRec, 15, 2);
    g.drawRoundedRectangle(filterRec, 15, 2);
    g.drawRoundedRectangle(envelopeRec, 15, 2);
    g.drawRoundedRectangle(modRec, 15, 2);
    g.drawRoundedRectangle(noiseRec, 15, 2);
    g.drawRect(zoneOne, 2);
    g.drawRect(zoneTwo, 2);


    g.setColour(juce::Colours::deepskyblue.brighter(0.6f));
    g.fillRoundedRectangle(sampleOneBackground, 15);

    g.setColour(juce::Colours::greenyellow.brighter(0.0f));
    g.fillRoundedRectangle(sampleTwoBackground, 15);

    g.setColour(juce::Colours::yellow.darker(0.2f));
    g.fillRoundedRectangle(sampleFusionBackground, 15);

    g.setColour(juce::Colours::white);
    g.setFont(28.0f);

    if (!loadedSampleOne) { g.drawFittedText("drag&drop sample here!", sampleOneBackground.getSmallestIntegerContainer(), juce::Justification::centred, 1); }
    if (!loadedSampleTwo) { g.drawFittedText("drag&drop sample here!", sampleTwoBackground.getSmallestIntegerContainer(), juce::Justification::centred, 1); }
    if (!loadedSampleTwo) { g.drawFittedText("Fusion of both samples", sampleFusionBackground.getSmallestIntegerContainer(), juce::Justification::centred, 1); }

    g.setColour(juce::Colours::black);
    g.drawFittedText("coming soon: modulation section", modRec.getSmallestIntegerContainer(), juce::Justification::centred, 1);

    g.setFont(lowPassSlider.getBounds().getWidth() / 5);

    g.setColour(juce::Colours::purple.brighter(0.5f));
    g.drawFittedText("LP", lowPassSlider.getBounds().withBottom(lowPassSlider.getBottom() - lowPassSlider.getTextBoxHeight()), juce::Justification::centred, 1);
    g.drawFittedText("HP", highPassSlider.getBounds().withBottom(highPassSlider.getBottom() - highPassSlider.getTextBoxHeight()), juce::Justification::centred, 1);
    g.drawFittedText("Q", lowQ_Slider.getBounds().withBottom(lowQ_Slider.getBottom() - lowQ_Slider.getTextBoxHeight()), juce::Justification::centred, 1);
    g.drawFittedText("Q", highQ_Slider.getBounds().withBottom(highQ_Slider.getBottom() - highQ_Slider.getTextBoxHeight()), juce::Justification::centred, 1);

    g.setColour(juce::Colours::hotpink.brighter(0.2f));
    g.drawFittedText("A", attackSlider.getBounds().withBottom(attackSlider.getBottom()-attackSlider.getTextBoxHeight()), juce::Justification::centred, 1);
    g.drawFittedText("D", decaySlider.getBounds().withBottom(decaySlider.getBottom() - decaySlider.getTextBoxHeight()), juce::Justification::centred, 1);
    g.drawFittedText("S", sustainSlider.getBounds().withBottom(sustainSlider.getBottom() - sustainSlider.getTextBoxHeight()), juce::Justification::centred, 1);
    g.drawFittedText("R", relaseSlider.getBounds().withBottom(relaseSlider.getBottom() - relaseSlider.getTextBoxHeight()), juce::Justification::centred, 1);

    g.setFont(attackSlider.getBounds().getWidth() / 6);
    g.drawFittedText("lin", curveSlider.getBounds().withHeight(curveSlider.getHeight() + 15), juce::Justification::centredBottom, 1);
    g.drawFittedText("log", sustainSlider.getBounds().withHeight(curveSlider.getHeight() + sustainSlider.getHeight() + 25).withLeft(sustainSlider.getX() + 25), juce::Justification::bottomLeft, 1);
    g.drawFittedText("exp", relaseSlider.getBounds().withHeight(curveSlider.getHeight() + sustainSlider.getHeight() + 25).translated(-25, 0), juce::Justification::bottomRight, 1);

    g.setColour(juce::Colours::black);
    g.setFont(masterGainSlider.getBounds().getWidth() / 5);
    g.drawFittedText("Gain", masterGainSlider.getBounds(), juce::Justification::centred, 1);

    //juce::Path p;
    //p.addEllipse(outputRec.getCentreX(), outputRec.getCentreY() + outputRec.getHeight() * 0.25f, 50, 50);
    //g.setColour(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    //g.fillPath(p);


    //add lock
    if (loadedSampleOne && !sampleLoader.isThreadRunning())
    {
        pointsOneLeft.clear();
        pointsOneRight.clear();

        auto ratio = fileBufferOne.getNumSamples() / (zoneOne.getWidth() - 20);
        auto bufferLeft = fileBufferOne.getReadPointer(0);
        auto bufferRight = fileBufferOne.getReadPointer(1);

        for (int i = 0; i < fileBufferOne.getNumSamples(); i += ratio)
        {
            pointsOneLeft.push_back(bufferLeft[i]);
            pointsOneRight.push_back(bufferRight[i]);
        }

        juce::Path pLeft;
        juce::Path pRight;

        auto startX = zoneOne.getX() + 10;
        auto startYLeft = zoneOne.getHeight() * 0.25f + 10;
        auto startYRight = zoneOne.getHeight() * 0.75f + 10;
        auto minLeft = zoneOne.getCentreY();    auto maxLeft = zoneOne.getY();
        auto minRight = zoneOne.getBottom();    auto maxRight = zoneOne.getCentreY();

        pLeft.startNewSubPath(startX, startYLeft);
        pRight.startNewSubPath(startX, startYRight);

        for (int i = 0; i < pointsOneLeft.size(); i++)
        {
            auto pointLeft = juce::jmap<float>(pointsOneLeft[i], -1.0f, 1.0f, minLeft, maxLeft);
            auto pointRight = juce::jmap<float>(pointsOneRight[i], -1.0f, 1.0f, minRight, maxRight);
            pLeft.lineTo(startX + i, pointLeft);
            pRight.lineTo(startX + i, pointRight);
        }
        g.strokePath(pLeft, juce::PathStrokeType(2));
        g.strokePath(pRight, juce::PathStrokeType(2));
    }


    if (loadedSampleTwo && !sampleLoader.isThreadRunning())
    {
        pointsTwoLeft.clear();
        pointsTwoRight.clear();
        
        auto ratio = fileBufferTwo.getNumSamples() / (zoneTwo.getWidth() - 20);
        auto bufferLeft = fileBufferTwo.getReadPointer(0);
        auto bufferRight = fileBufferTwo.getReadPointer(1);

        for (int i = 0; i < fileBufferTwo.getNumSamples(); i += ratio)
        {
            pointsTwoLeft.push_back(bufferLeft[i]);
            pointsTwoRight.push_back(bufferRight[i]);
        }

        juce::Path pLeft;
        juce::Path pRight;

        auto startX = zoneTwo.getX() + 10;
        auto startYLeft = zoneTwo.getHeight() * 0.25f + zoneOne.getHeight() + 20;
        auto startYRight = zoneTwo.getHeight() * 0.75f + zoneOne.getHeight() + 20;
        auto minLeft = zoneTwo.getCentreY();    auto maxLeft = zoneTwo.getY();
        auto minRight = zoneTwo.getBottom();    auto maxRight = zoneTwo.getCentreY();

        pLeft.startNewSubPath(startX, startYLeft);
        pRight.startNewSubPath(startX, startYRight);

        for (int i = 0; i < pointsTwoLeft.size(); i++)
        {
            auto pointLeft = juce::jmap<float>(pointsTwoLeft[i], -1.0f, 1.0f, minLeft, maxLeft);
            auto pointRight = juce::jmap<float>(pointsTwoRight[i], -1.0f, 1.0f, minRight, maxRight);
            pLeft.lineTo(startX + i, pointLeft);
            pRight.lineTo(startX + i, pointRight);
        }
        g.strokePath(pLeft, juce::PathStrokeType(2));
        g.strokePath(pRight, juce::PathStrokeType(2));
    }

 
}

void SampleBasedSynthAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..

    //Layout 
    sampleRec = juce::Rectangle<float>(10, 10, getWidth() * 0.75 - 10, getHeight() * 0.45 - 10);
    noiseRec = juce::Rectangle<float>(sampleRec.getRight() + 10, 10, getWidth() * 0.15 - 10, getHeight() * 0.45 - 10);
    outputRec = juce::Rectangle<float>(noiseRec.getRight() + 10, 10, getWidth() * 0.10 - 20, getHeight() - 20);
    filterRec = juce::Rectangle<float>(10, sampleRec.getBottom() + 10, getWidth() * 0.20 - 10, getHeight() * 0.45 - 10);
    envelopeRec = juce::Rectangle<float>(filterRec.getRight() + 10, sampleRec.getBottom() + 10, getWidth() * 0.20 - 10, getHeight() * 0.45 - 10);
    modRec = juce::Rectangle<float>(envelopeRec.getRight() + 10, sampleRec.getBottom() + 10, getWidth() * 0.50 - 10, getHeight() * 0.45 - 10);

    //drag&drop zones
    zoneOne = juce::Rectangle<float>(15, 15, getWidth() * 0.55 - 10, getHeight() * 0.15 - 15);
    zoneTwo = juce::Rectangle<float>(15, zoneOne.getBottom() + 10, getWidth() * 0.55 - 10, getHeight() * 0.15 - 15);

    //background for sample wav form
    sampleOneBackground = juce::Rectangle<float>(zoneOne);
    sampleTwoBackground = juce::Rectangle<float>(zoneTwo);
    sampleFusionBackground = juce::Rectangle<float>(15, zoneTwo.getBottom() + 10, zoneTwo.getWidth(), zoneTwo.getHeight());
    
    //sample 1
    sampleOneAsCnv.setBounds(sampleOneBackground.getRight() +10, sampleOneBackground.getY() + 5, sampleOneBackground.getHeight() / 2 - 5, sampleOneBackground.getHeight() / 2 - 5);
    sampleOneInCnv.setBounds(sampleOneBackground.getRight() +10, sampleOneAsCnv.getBottom() + 5, sampleOneBackground.getHeight() / 2 - 5, sampleOneBackground.getHeight() / 2 - 5);
    sampleOneGain.setBounds(sampleOneAsCnv.getRight(), sampleOneBackground.getY(), sampleOneBackground.getHeight() - 5, sampleOneBackground.getHeight() - 5);
    sampleOneMute.setBounds(sampleOneGain.getRight(), sampleOneBackground.getY() + 20, sampleOneBackground.getHeight() - 40, sampleOneBackground.getHeight() - 40);
    sampleOneReverse.setBounds(sampleOneMute.getRight() + 10, sampleOneBackground.getY() + 20, sampleOneBackground.getHeight() - 40, sampleOneBackground.getHeight() - 40);
    
    //sample 2
    sampleTwoAsCnv.setBounds(sampleTwoBackground.getRight() + 10, sampleTwoBackground.getY() + 5, sampleTwoBackground.getHeight() / 2 - 5, sampleTwoBackground.getHeight() / 2 - 5);
    sampleTwoInCnv.setBounds(sampleTwoBackground.getRight() + 10, sampleTwoAsCnv.getBottom() + 5, sampleTwoBackground.getHeight() / 2 - 5, sampleTwoBackground.getHeight() / 2 - 5);    
    sampleTwoGain.setBounds(sampleTwoAsCnv.getRight(), sampleTwoBackground.getY(), sampleTwoBackground.getHeight() - 5, sampleTwoBackground.getHeight() - 5);
    sampleTwoMute.setBounds(sampleTwoGain.getRight(), sampleTwoBackground.getY() + 20, sampleTwoBackground.getHeight() - 40, sampleTwoBackground.getHeight() - 40);
    sampleTwoReverse.setBounds(sampleTwoMute.getRight() + 10, sampleTwoBackground.getY() + 20, sampleTwoBackground.getHeight() - 40, sampleTwoBackground.getHeight() - 40);

    //fusion sample
    fusionSampleAsCnv.setBounds(sampleFusionBackground.getRight() + 10, sampleFusionBackground.getY() + 5, sampleFusionBackground.getHeight() / 2 - 5, sampleFusionBackground.getHeight() / 2 - 5);
    fusionSampleInCnv.setBounds(sampleFusionBackground.getRight() + 10, fusionSampleAsCnv.getBottom() + 5, sampleFusionBackground.getHeight() / 2 - 5, sampleFusionBackground.getHeight() / 2 - 5);
    fusionSampleGain.setBounds(fusionSampleAsCnv.getRight(), sampleFusionBackground.getY(), sampleFusionBackground.getHeight() - 5, sampleFusionBackground.getHeight() - 5);
    fusionSampleMute.setBounds(fusionSampleGain.getRight(), sampleFusionBackground.getY() + 20, sampleFusionBackground.getHeight() - 40, sampleFusionBackground.getHeight() - 40);
    fusionSampleReverse.setBounds(fusionSampleMute.getRight() + 10, sampleFusionBackground.getY() + 20, sampleFusionBackground.getHeight() - 40, sampleFusionBackground.getHeight() - 40);

    //noise
    noiseType.setBounds(noiseRec.getX() + 15, noiseRec.getY() + 5, noiseRec.getWidth() - 30, noiseRec.getHeight() * 0.15 - 10);
    noiseGainSlider.setBounds(noiseRec.getX() + 15, noiseType.getBottom() + 10, noiseRec.getWidth() - 30, noiseRec.getHeight() * 0.5 - 10);
    combFilterSlider.setBounds(noiseRec.getX() + 15, noiseGainSlider.getBottom() + 10, noiseRec.getWidth() - 30, noiseRec.getHeight() * 0.15 - 10);
    noiseInCnv.setBounds(noiseRec.getX() + noiseRec.getWidth() * 0.25 - (noiseRec.getHeight() * 0.2 - 10) / 2, combFilterSlider.getBottom() + 10, noiseRec.getHeight() * 0.2 - 10, noiseRec.getHeight() * 0.2 - 10);
    noiseMute.setBounds(noiseRec.getX() + noiseRec.getWidth() * 0.75 - (noiseRec.getHeight() * 0.2 - 10) / 2, combFilterSlider.getBottom() + 10, noiseRec.getHeight() * 0.2 - 10, noiseRec.getHeight() * 0.2 - 10);



    //filter
    lowPassSlider.setBounds(filterRec.getX() + 5, filterRec.getY() + 5, filterRec.getWidth() / 2 - 10, filterRec.getHeight() * 0.40 - 20);
    highPassSlider.setBounds(filterRec.getX() + 5 + filterRec.getWidth() / 2, filterRec.getY() + 5, filterRec.getWidth() / 2 - 10, filterRec.getHeight() * 0.40 - 20);
    lowQ_Slider.setBounds(filterRec.getX() + 5, lowPassSlider.getBottom() + 10, filterRec.getWidth() / 2 - 10, filterRec.getHeight() * 0.40 - 20);
    highQ_Slider.setBounds(filterRec.getX() + 5 + filterRec.getWidth() / 2, lowPassSlider.getBottom() + 10, filterRec.getWidth() / 2 - 10, filterRec.getHeight() * 0.40 - 20);
    lowCutSlopeBox.setBounds(filterRec.getX() + 20, lowQ_Slider.getBottom() + 20, filterRec.getWidth()/2 - 40, filterRec.getHeight() * 0.20 - 20);
    highCutSlopeBox.setBounds(filterRec.getX() + 20 + filterRec.getWidth() / 2, lowQ_Slider.getBottom() + 20, filterRec.getWidth()/2 - 40, filterRec.getHeight() * 0.20 - 20);

    //envelope
    attackSlider.setBounds(envelopeRec.getX() + 5, envelopeRec.getY() + 5, envelopeRec.getWidth() / 2 - 10, envelopeRec.getHeight() * 0.40 - 20);
    decaySlider.setBounds(envelopeRec.getX() + 5 + envelopeRec.getWidth() / 2, envelopeRec.getY() + 5, envelopeRec.getWidth() / 2 - 10, envelopeRec.getHeight() * 0.40 - 20);
    sustainSlider.setBounds(envelopeRec.getX() + 5, attackSlider.getBottom() + 10, envelopeRec.getWidth() /2 - 10, envelopeRec.getHeight() * 0.40 - 20);
    relaseSlider.setBounds(envelopeRec.getX() + 5 + envelopeRec.getWidth() / 2, attackSlider.getBottom() + 10, envelopeRec.getWidth() / 2 - 10, envelopeRec.getHeight() * 0.40 - 20);
    curveSlider.setBounds(envelopeRec.getX() + 20, sustainSlider.getBottom() + 10, envelopeRec.getWidth() - 40, envelopeRec.getHeight() * 0.20 - 20);

    //output
    levelMeterLeft.setBounds(outputRec.getX() + 5, outputRec.getY() + 5, (outputRec.getWidth() / 2) - 10, getHeight() * 0.80 - 20);
    levelMeterRight.setBounds(outputRec.getX() + 5 + outputRec.getWidth() / 2, outputRec.getY() + 5, outputRec.getWidth() / 2 - 10, getHeight() * 0.80 - 20);
    masterGainSlider.setBounds(outputRec.getX() + 5, levelMeterLeft.getBottom() + 10, outputRec.getWidth() - 10, outputRec.getHeight() * 0.20 - 20);

}

bool SampleBasedSynthAudioProcessorEditor::isInterestedInFileDrag(const juce::StringArray& files)
{
    for (auto file : files)
    {
        if (file.contains(".wav") || file.contains(".mp3") || file.contains(".aif"))
        {
            return true;
        }
    }
    return false;
}

void SampleBasedSynthAudioProcessorEditor::filesDropped(const juce::StringArray& files, int x, int y)
{   
    if(!sampleLoader.isThreadRunning())
    {
        for (auto file : files)
        {
            if (isInterestedInFileDrag(file))
            {
                if (zoneOne.contains(x, y))
                {
                    sampleLoader.loadSample(file, fileBufferOne, audioProcessor.getSynthOne());
                    loadedSampleOne = true;
                    return;
                }
                if (zoneTwo.contains(x, y))
                {
                    sampleLoader.loadSample(file, fileBufferTwo, audioProcessor.getSynthTwo());
                    loadedSampleTwo = true;
                    return;
                }
            }
        }
    }
}


void SampleBasedSynthAudioProcessorEditor::timerCallback()
{
    levelMeterLeft.setLevel(audioProcessor.getRmsValueLeft());
    levelMeterRight.setLevel(audioProcessor.getRmsValueRight());
    levelMeterLeft.repaint();
    levelMeterRight.repaint();
}

void SampleBasedSynthAudioProcessorEditor::actionListenerCallback(const juce::String& message)
{
    if (message.contains("loaded sample"))
    {
        repaint();
    }
}



