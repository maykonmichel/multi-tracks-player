#pragma once

#include <JuceHeader.h>
#include "ReferenceCountedBuffer.h"

class AudioDownload : public Thread {
public:
    AudioDownload();

    ReferenceCountedBuffer::Ptr currentBuffer;
    String chosenPath;

private:
    void run() override;

    void checkForBuffersToFree();

    void checkForPathToOpen();

    AudioFormatManager formatManager;
    ReferenceCountedArray<ReferenceCountedBuffer> buffers;
};
