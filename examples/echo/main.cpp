#include "audio.hpp"
#include <cassert>
#include <cstdio>
#include <memory>
#include <portaudio.h>
#include <unistd.h>

using namespace aud;

int main() {
    initialize();
    auto vdsp = std::make_shared<VolumeDSP>();
    mic->dsps.push_back(std::make_shared<RnnoiseDSP>());

    aud::Player p(mic);
    p.start();
    while (1) {
        Pa_Sleep(5000);
        p.stop();
        Pa_Sleep(1000);
        p.start();
    }
}