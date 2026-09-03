#pragma once
#include <atomic>
#include <string>
#include <thread>
#include <algorithm>
#include <portaudio.h>
#include "pa_ringbuffer.h"
#include "model/track.hpp"

/*

This file is the header for the player class. 
It opens the audio stream and decodes and plays files.
Most of the code was taken from
https://www.youtube.com/watch?v=-jugPJ_O8iM
and was updated for today's ffmpeg libraries

*/

class Player {
public:
    enum class PlaybackState {
        Stopped,
        Playing,
        Paused,
        Seeking
    };

    Player();
    ~Player();

    void play(const Track& track);

    void stop();

    void pause();
    void resume();
    void togglePause();

    void seekTo(double seconds);
    void seekBy(double seconds);

    void setVolume(float vol) {
        vol = std::clamp(vol, 0.0f, 1.0f);
        volume.store(vol);
    };
    float getVolume() const { return volume; }

    PlaybackState getPlaybackState() const { return state; }
    double getPosition() const;

    bool consumeFinished();
private:
    std::string filePath;

    const PaDeviceInfo* deviceInfo;

    PaStream* stream = nullptr;

    PaUtilRingBuffer ringBuffer;
    char* ringBufferData = nullptr;

    std::thread decodeThread;

    std::atomic<PlaybackState> state{PlaybackState::Stopped};
    std::atomic<bool> stopRequested{false};
    std::atomic<bool> decoderFinished{false};
    std::atomic<bool> playbackFinished{false};

    std::atomic<int64_t> seekRequest{-1};

    std::atomic<float> volume{1.0f};

    static int portAudioCallback(
        const void* inputBuffer, void* outputBuffer,
        unsigned long framesPerBuffer,
        const PaStreamCallbackTimeInfo* timeInfo,
        PaStreamCallbackFlags statusFlags,
        void* userData
    );

    int processAudio(float* output, unsigned long framesPerBuffer);

    // for sending to spectrum analyzer
    std::vector<float> mono_queue;

    void decodeLoop();
};
