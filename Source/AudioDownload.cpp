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
        File file(pathToOpen);
        std::unique_ptr<AudioFormatReader> reader(formatManager.createReaderFor(file));

        if (reader.get() != nullptr) {
            auto duration = reader->lengthInSamples / reader->sampleRate;

            if (duration < 2) {
                ReferenceCountedBuffer::Ptr newBuffer = new ReferenceCountedBuffer(file.getFileName(),
                                                                                   (int) reader->numChannels,
                                                                                   (int) reader->lengthInSamples);

                reader->read(newBuffer->getAudioSampleBuffer(), 0, (int) reader->lengthInSamples, 0, true, true);
                currentBuffer = newBuffer;
                buffers.add(newBuffer);
            } else {
                // handle the error that the file is 2 seconds or longer..
            }
        }
    }
}
