#include "audio.hpp"
#include "portaudiocpp/BlockingStream.hxx"
#include "portaudiocpp/DirectionSpecificStreamParameters.hxx"
#include <cassert>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <thread>

using namespace aud;

Player::Player(std::shared_ptr<Source> src) : src(src) {
    init();
}

void Player::init() {
    assert(src);
    assert(0 < src->channels() && src->channels() <= getOutputDevice().maxOutputChannels());
    portaudio::DirectionSpecificStreamParameters outParams;
    outParams.setDevice(getOutputDevice());
    outParams.setNumChannels(src->channels());
    outParams.setSampleFormat(portaudio::FLOAT32);
    outParams.setHostApiSpecificStreamInfo(nullptr);
    outParams.setSuggestedLatency(getOutputDevice().defaultHighOutputLatency());
    size_t fs = src->frameSize();
    if (fs == 0)
        fs = paFramesPerBufferUnspecified;
    portaudio::StreamParameters params(
        portaudio::DirectionSpecificStreamParameters::null(),
        outParams,
        SAMPLE_RATE,
        fs,
        paNoFlag
    );
    stream.open(params);
    thrd = std::thread(&Player::tfunc, this);
    // thrd.detach();
}

void Player::start() {
    src->start();
}

void Player::stop() {
    src->stop();
}

void Player::term() {
    if (!stream.isStopped())
        stop();
    if (stream.isOpen())
        stream.close();
}

void Player::setVolume(float percentage) {
    volume = percentage / 100;
}

void Player::tfunc() {
    const size_t defaultFrameSize = 64;
    const size_t SIZE = 480;
    float buf[SIZE];
    stream.start();
    while (1) {
        src->stateMux.lock();
        switch (src->state()) {
        case Source::State::Active: {
            size_t n = src->frameSize();
            assert(n <= SIZE);
            if (n == 0) {
                size_t nr = src->available();
                size_t nw = stream.availableWriteSize();
                if (nr == 0) {
                    if (nw != 0) {
                        nr = nw;
                    } else {
                        nr = defaultFrameSize;
                    }
                } else if (nw == 0) {
                    nw = nr;
                }
                using std::min;
                n = min(nr, min(nw, SIZE / src->channels()));
            }
            // printf("%zu\n", n);
            n = src->read(buf, n);
            src->stateMux.unlock();
            float vol = volume;
            if (vol != 1) {
                for (size_t i = 0; i < n; i++) {
                    buf[i] *= volume;
                }
            }
            try {
                stream.write(buf, n);
            } catch (...) {
            }
        } break;

        case Source::State::Stopped: {
            src->stateMux.unlock();

            this->lock();
            stream.stop();
            this->unlock();
            src->waitStart();
            this->lock();
            stream.start();
            this->unlock();
        } break;

        case Source::State::Finalized: {
            src->stateMux.unlock();
            if (endOfSourceCallback) {
                endOfSourceCallback(this);
            }
            this->term();
            return;
        } break;
        }
    }
}