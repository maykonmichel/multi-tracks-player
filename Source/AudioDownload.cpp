#include "AudioDownload.h"

AudioDownload::AudioDownload() : Thread("Background Thread") {
    formatManager.registerBasicFormats();
}

void AudioDownload::run() {
    while (!threadShouldExit()) {
        checkForPathToOpen();
        checkForBuffersToFree();
        wait(500);
    }
}

void AudioDownload::checkForBuffersToFree() {
    for (auto i = buffers.size(); --i >= 0;) {
        ReferenceCountedBuffer::Ptr buffer(buffers.getUnchecked(i));

        if (buffer->getReferenceCount() == 2)
            buffers.remove(i);
    }
}

void AudioDownload::checkForPathToOpen() {
    String pathToOpen;
    pathToOpen.swapWith(chosenPath);

    if (pathToOpen.isNotEmpty()) {
        URL url(pathToOpen);
        auto *file = url.createInputStream(false);
        std::unique_ptr<AudioFormatReader> reader(formatManager.createReaderFor(file));

        if (reader.get() != nullptr) {
            ReferenceCountedBuffer::Ptr newBuffer = new ReferenceCountedBuffer("file.getFileName()",
                                                                               (int) reader->numChannels,
                                                                               (int) reader->lengthInSamples);

            reader->read(newBuffer->getAudioSampleBuffer(), 0, (int) reader->lengthInSamples, 0, true, true);
            currentBuffer = newBuffer;
            buffers.add(newBuffer);
        }
    }
}
