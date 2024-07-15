#include "audio.hpp"
#include "portaudiocpp/DirectionSpecificStreamParameters.hxx"
#include "portaudiocpp/SampleDataFormat.hxx"
#include <cassert>
#include <cstring>
#include <mutex>
#include <portaudio.h>
#include <rnnoise.h>

using namespace aud;

size_t Recorder::getFrameSize() {
    return (size_t)rnnoise_get_frame_size();
}

void Recorder::init() {
    portaudio::DirectionSpecificStreamParameters inParams;
    inParams.setDevice(getInputDevice());
    inParams.setNumChannels(1);
    inParams.setSampleFormat(portaudio::FLOAT32);
    inParams.setHostApiSpecificStreamInfo(nullptr);
    inParams.setSuggestedLatency(getInputDevice().defaultLowInputLatency());
    portaudio::StreamParameters params(
        inParams,
        portaudio::DirectionSpecificStreamParameters::null(),
        SAMPLE_RATE,
        (unsigned long)Recorder::getFrameSize(),
        paNoFlag
    );
    this->lock();
    stream.open(params);
    setState(State::Stopped);
    this->unlock();
}

void Recorder::setState(Source::State state) {
    this->stateMux.lock();
    st = state;
    this->stateMux.unlock();
}

void Recorder::start() {
    this->lock();
    stream.start();
    this->unlock();
    setState(State::Active);
    cv.notify_all();
}

void Recorder::stop() {
    setState(State::Stopped);
    this->lock();
    stream.stop();
    this->unlock();
}

void Recorder::term() {
    if (!stream.isStopped()) {
        stream.stop();
    }
    this->lock();
    setState(State::Finalized);
    stream.close();
    this->unlock();
    cv.notify_all();
}

int Recorder::channels() const {
    return 1;
}

Recorder::State Recorder::state() {
    this->lock();
    State res = st;
    this->unlock();
    return res;
}

size_t Recorder::available() {
    this->lock();
    size_t res = stream.availableReadSize();
    this->unlock();
    return res;
}

size_t Recorder::read(float frame[], size_t num) {
    assert(num == getFrameSize());
    this->lock();
    stream.read(frame, num);
    this->unlock();
    for (auto &dsp : dsps) {
        dsp->process(frame);
    }
    return num;
}

void Recorder::waitStart() {
    this->lock();
    if (st == State::Stopped) {
        this->unlock();
        std::unique_lock<std::mutex> lk(cvMux);
        cv.wait(lk);
    } else {
        this->unlock();
    }
}

size_t Recorder::frameSize() {
    return Recorder::getFrameSize();
}