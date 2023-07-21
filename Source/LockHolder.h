/*
  ==============================================================================

    LockHolder.h
    Created: 7 Jun 2023 2:49:25pm
    Author:  Marvin

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"



class LockHolder : public juce::Thread, public juce::ActionBroadcaster
{
public:
    LockHolder() : juce::Thread("hold lock") {}
    ~LockHolder() {}

    void run() override
    {
        {
            const juce::GenericScopedLock<juce::CriticalSection> myScopedLock(*lockingThread.get());
            sendActionMessage(message);

            while (runningThread->isThreadRunning())
            {
                wait(200);
            }
            runningThread.release();
            lockingThread.release();
        }
        runningThread.reset({ nullptr });
        lockingThread.reset({ nullptr });
    }

    void holdLockWhileRunning(const juce::String m, juce::CriticalSection& lt, juce::Thread& rt)
    {
        message = m;
        runningThread.reset(&rt);
        lockingThread.reset(&lt);
        startThread(Priority::low);
    }


private:

    juce::String message;

    std::unique_ptr<juce::Thread> runningThread{ nullptr };
    std::unique_ptr<juce::CriticalSection> lockingThread { nullptr };

};