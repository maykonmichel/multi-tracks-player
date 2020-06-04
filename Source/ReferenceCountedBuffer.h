#pragma once

#include <JuceHeader.h>

class ReferenceCountedBuffer : public ReferenceCountedObject {
public:
    typedef ReferenceCountedObjectPtr<ReferenceCountedBuffer> Ptr;

    ReferenceCountedBuffer(String nameToUse, int numChannels, int numSamples);

    ~ReferenceCountedBuffer();

    AudioSampleBuffer *getAudioSampleBuffer();

    int position = 0;
private:
    String name;
    AudioSampleBuffer buffer;
};
