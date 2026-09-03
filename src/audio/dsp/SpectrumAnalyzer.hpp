#pragma once

#include <fftw3.h>
#include <portaudio.h>
#include "pa_ringbuffer.h"

#include <vector>

/*
 * This class is going to process and help display FFT data for ftxui
 *
 * It will output an array of floats to build the graph
 *
 */

class SpectrumAnalyzer {
public:
	SpectrumAnalyzer();
	~SpectrumAnalyzer();
	void loadData(std::vector<float> samples);
	void processFFT();
	std::vector<float> getFFT();
	void flushBuffers();
private:
	float* in;
	float* out;
	fftwf_plan p;	
	std::size_t writeIndex = 0;
	int FFT_SIZE = 4096;
	int startIndex;
	int endIndex;
	static constexpr std::size_t DISPLAY_BINS = 20;	// from foobars 20 bar spectrum
	int freqStart = 20;
	int freqEnd = 20000;
	float sampleRate;
	double binPerHz;

	// ring buffer for FFT calculations
	PaUtilRingBuffer ringBufferSpectrum;
	char* ringBufferData = nullptr;
};
