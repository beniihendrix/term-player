#pragma once
#include <atomic>
#include <string>
#include <thread>
#include <portaudio.h>
#include "pa_ringbuffer.h"
#include "track.h"

/*

This file is the header for the player class. 
It opens the audio stream and decodes and plays files.
Most of the code was taken from
https://www.youtube.com/watch?v=-jugPJ_O8iM
and was updated for today's ffmpeg libraries

*/

class Player{
private:
    std::string filePath;
    PaStream* stream = nullptr;

    PaUtilRingBuffer ringBuffer;
    char* ringBufferData = nullptr; // raw memory for the ring buffer

    std::thread decodeThread;
    std::atomic<bool> isPlaying{false};

    static int portAudioCallback(
        const void* inputBuffer, void* outputBuffer,
        unsigned long framesPerBuffer,
        const PaStreamCallbackTimeInfo* timeInfo,
        PaStreamCallbackFlags statusFlags,
        void* userData
    );
    
    int processAudio(float* output, unsigned long framesPerBuffer);
    
    void decodeLoop();
public:
    Player();
    ~Player();

    void play(const Track& track);
    void stop();
};
