/*
  ==============================================================================

    ThreadManager.h
    Created: 9 Jun 2023 10:17:15pm
    Author:  Marvin

  ==============================================================================
*/

#pragma once


#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "SampleLoader.h"
#include "PreProcessor.h"
#include "SynthSoundUpdater.h"
#include "LockHolder.h"


class ThreadManager : public juce::Thread
{
public:
    ThreadManager() : juce::Thread("lock holder") {}
    ~ThreadManager() {}

    void loadSample(juce::String& p, juce::AudioBuffer<float>& b, const juce::String& m)
    {
        threadToRun.reset(&sampleLoader);
        threadToLock.reset(&preProcessor);
        startThread();
        sampleLoader.loadSample(p, b, m);
        notify();
    }

    void run() override
    {
        {
            const juce::GenericScopedLock<juce::CriticalSection> myScopedLock(*threadToLock.get());

            wait(-1);

            while (threadToRun->isThreadRunning())
            {
                wait(200);
            }

            threadToRun.release();
            threadToLock.release();
        }
        //threadToRun.reset({ nullptr });
        //threadToLock.reset({ nullptr });
    }

private:
    
    SampleLoader sampleLoader;
    PreProcessor preProcessor;
    SynthSoundUpdater updater;

    std::unique_ptr<juce::Thread> threadToRun{ nullptr };
    std::unique_ptr<juce::CriticalSection> threadToLock { nullptr };
};