#pragma once

#include <fftw3.h>

#include <vector>

/*
 * This class is going to process and help display FFT data for ftxui
 *
 * It will output an array of integers to build the graph
 *
 */

class SpectrumAnalyzer {
public:
	SpectrumAnalyzer();
	~SpectrumAnalyzer();
	void processFFT();
	std::vector<float> getFFT();
private:
	float* in;
	float* out;
	fftw_plan p;
	int startIndex;
	int spectroSize;
};
