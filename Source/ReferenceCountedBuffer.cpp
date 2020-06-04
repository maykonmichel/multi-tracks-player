#include "ReferenceCountedBuffer.h"

#include <utility>

ReferenceCountedBuffer::ReferenceCountedBuffer(
        String nameToUse,
        int numChannels,
        int numSamples) :
        name(std::move(nameToUse)),
        buffer(numChannels, numSamples) {
    DBG (String("Buffer named '") + name +
         "' constructed. numChannels = " + String(numChannels) +
         ", numSamples = " + String(numSamples));
}

ReferenceCountedBuffer::~ReferenceCountedBuffer() {
    DBG (String("Buffer named '") + name + "' destroyed");
}

AudioSampleBuffer *ReferenceCountedBuffer::getAudioSampleBuffer() {
    return &buffer;
}
