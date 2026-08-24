#include "SpectrumAnalyzer.hpp"

SpectrumAnalyzer::SpectrumAnalyzer() {
	// create fftw plan
	// define sample ratio, start and end index
	
	// sample ratio = frames per buffer / sample rate
	// start index = sample ratio * freqstart (perhaps with ceiling func)
	// end index = sample ratio * freqend (with maximum of frames per buffer / 2)
	//
}

SpectrumAnalyzer::~SpectrumAnalyzer() {
	// destroy plan
	// delete buffers
}

void SpectrumAnalyzer::loadData(float sample) {
	in[writeIndex++] = sample;

	if (writeIndex == FFT_SIZE)
	{
		// collected full fft window
		// signal that data is ready
		writeIndex = 0;
	}
	// TODO: make flexible number of channels

	// quick for loop (maybe should find a way to throw this in
	// with the volume function
	// TODO: Should definitely make inputs references rather than copies
}

void SpectrumAnalyzer::processFFT() {
	/*
	 * What processFFT needs
	 * FRAMES_PER_BUFFER
	 * Needs float input buffer (decode loops output)
	 * Float output buffer (not sure if data will be changed)
	 */

	// give a call to fftw_execute(pointer to plan)
}

std::vector<float> SpectrumAnalyzer::getFFT() {
	std::vector<float> fftBins;

	// choose set number of bins
	// proportion = i / (double) displaysize
	// proportion decides the real bin pull (like for log scale)
	// do a for loop of set bin size and push back
	// loading: freq = output[start index + prop * spectroSize]
	// should consider solving for dB and doing a log scale

	return fftBins;
}

void SpectrumAnalyzer::flushBuffers() {
}
