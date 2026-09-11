#pragma once

#include <fftw3.h>
#include <portaudio.h>
#include "audio/pa_ringbuffer.h"

#include <vector>
#include <span>
#include <thread>

/*
 * This class is going to process and help display FFT data for ftxui
 *
 * It will output an array of floats to build the graph
 *
 */

class SpectrumAnalyzer {
public:
	static constexpr std::size_t FFT_SIZE = 4096;
    static constexpr std::size_t HOP_SIZE = FFT_SIZE / 2;
    static constexpr std::size_t FFT_BINS = FFT_SIZE / 2 + 1;
    static constexpr std::size_t DISPLAY_BINS = 20;

	SpectrumAnalyzer(double deviceSampRate);
	~SpectrumAnalyzer();
	
	// fft handling
	void loadData(
		std::span<const float> samples
	) noexcept;
	void processFFT(std::array<float, HOP_SIZE>& hop);
	void workerLoop();
	void flushBuffers();

	// for UI display
	std::array<float, SpectrumAnalyzer::DISPLAY_BINS> getGraph() const;

	// member setters
	void setStopRequested(bool stop) { stopRequested.store(stop); }
private:
	// fft info
	fftwf_plan p = nullptr;
	fftwf_complex* fftOutput = nullptr;

	// frequency info
	static constexpr double freqStart = 20.0;
	static constexpr double freqEnd = 20000.0;

	int startIndex = 0;
	int endIndex = 0;
	float sampleRate = 0;
	double hzPerBin = 0.0;

	std::array<std::size_t, DISPLAY_BINS + 1> graphEdges;

	// worker thread
	std::thread t;
	std::atomic<bool> stopRequested{false};

	// FFT samples
	std::array<float, FFT_SIZE> window{};
	std::array<float, FFT_SIZE> windowedInput{};
	std::array<float, FFT_SIZE> hannWindow{};
	float windowSum = 0.0f;

	// published UI data
	std::array<float, DISPLAY_BINS> graphBins{};
	mutable std::mutex graphMutex;

	// ring buffer for FFT calculations
	PaUtilRingBuffer ringBufferSpectrum;
	char* ringBufferData = nullptr;

};
