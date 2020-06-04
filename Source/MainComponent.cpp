#include "MainComponent.h"

MainComponent::MainComponent() {
    setSize(300, 200);

    addAndMakeVisible(openButton);
    openButton.setButtonText("Open...");
    openButton.onClick = [this] { openButtonClicked(); };

    addAndMakeVisible(clearButton);
    clearButton.setButtonText("Clear");
    clearButton.onClick = [this] { clearButtonClicked(); };

    setAudioChannels(0, 2);

    audioDownload.startThread();
}

MainComponent::~MainComponent() {
    audioDownload.stopThread(4000);
    shutdownAudio();
}

void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate) {
}

void MainComponent::getNextAudioBlock(const AudioSourceChannelInfo &bufferToFill) {
    ReferenceCountedBuffer::Ptr retainedCurrentBuffer(audioDownload.currentBuffer);

    if (retainedCurrentBuffer == nullptr) {
        bufferToFill.clearActiveBufferRegion();
        return;
    }

    auto *currentAudioSampleBuffer = retainedCurrentBuffer->getAudioSampleBuffer();
    auto position = retainedCurrentBuffer->position;

    auto numInputChannels = currentAudioSampleBuffer->getNumChannels();
    auto numOutputChannels = bufferToFill.buffer->getNumChannels();

    auto outputSamplesRemaining = bufferToFill.numSamples;
    auto outputSamplesOffset = 0;

    while (outputSamplesRemaining > 0) {
        auto bufferSamplesRemaining = currentAudioSampleBuffer->getNumSamples() - position;
        auto samplesThisTime = jmin(outputSamplesRemaining, bufferSamplesRemaining);

        for (auto channel = 0; channel < numOutputChannels; ++channel) {
            bufferToFill.buffer->copyFrom(channel,
                                          bufferToFill.startSample + outputSamplesOffset,
                                          *currentAudioSampleBuffer,
                                          channel % numInputChannels,
                                          position,
                                          samplesThisTime);
        }

        outputSamplesRemaining -= samplesThisTime;
        outputSamplesOffset += samplesThisTime;
        position += samplesThisTime;

        if (position == currentAudioSampleBuffer->getNumSamples())
            position = 0;
    }

    retainedCurrentBuffer->position = position;
}

void MainComponent::releaseResources() {
}

void MainComponent::paint(Graphics &g) {
    g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));
}

void MainComponent::resized() {
    openButton.setBounds(10, 10, getWidth() - 20, 20);
    clearButton.setBounds(10, 40, getWidth() - 20, 20);
}

void MainComponent::openButtonClicked() {
    FileChooser chooser("Select a Wave file shorter than 2 seconds to play...",
                        {},
                        "*.wav");

    if (chooser.browseForFileToOpen()) {
        auto file = chooser.getResult();
        auto path = file.getFullPathName();
        audioDownload.chosenPath.swapWith(path);
        audioDownload.notify();
    }
}

void MainComponent::clearButtonClicked() {
    audioDownload.currentBuffer = nullptr;
}
