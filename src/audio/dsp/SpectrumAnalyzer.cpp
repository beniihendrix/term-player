#include "SpectrumAnalyzer.hpp"

#include <algorithm>
#include <numbers>
#include <numeric>

SpectrumAnalyzer::SpectrumAnalyzer(double deviceSampRate) {
	// allocating spectrum ring buffer memory
	unsigned int numFrames = 8192 / 2;		// since we're mono
    unsigned int bytesPerSample = sizeof(float);
    ringBufferData = new char[numFrames * bytesPerSample];
    PaUtil_InitializeRingBuffer(
		&ringBufferSpectrum,
		bytesPerSample,
		numFrames,
		ringBufferData
	);
	
	// define graph paramters
	sampleRate = deviceSampRate;
	hzPerBin = sampleRate / FFT_SIZE;
	startIndex = static_cast<std::size_t>(
		std::ceil(hzPerBin * freqStart)
	);
	endIndex = std::floor(freqEnd / hzPerBin);
	endIndex = std::min(
		static_cast<float>(endIndex),
		static_cast<float>(FFT_SIZE / 2)
	);	// for nyquist

	// generate graph edges (defined boundaries to pull)
	for (std::size_t i = 0; i <= DISPLAY_BINS; i++)
	{
		// calculate proportion
		double proportion = 
			static_cast<double>(i) / 
			static_cast<double>(DISPLAY_BINS);

		// calculate frequency to pull
		double frequency =
			freqStart *
			std::pow(
				freqEnd / freqStart,
				proportion
			);

		// idk what this does
		std::size_t fftBin =
			static_cast<std::size_t>(
				std::round(frequency / hzPerBin)
			);

		// idk what this does
		graphEdges[i] = std::min(
			fftBin,
			static_cast<std::size_t>(FFT_SIZE / 2)
		);
	}

	// compute hanning window
	for (std::size_t i = 0; i < FFT_SIZE; i++)
	{
		hannWindow[i] =
			0.5f *
			(1.0f -
				std::cos(
					2.0f *
					std::numbers::pi_v<float> *
					i /
					static_cast<float>(FFT_SIZE - 1)
				));
	}

	windowSum = std::accumulate(
		hannWindow.begin(),
		hannWindow.end(),
		0.0f
	);

	// allocate memory for output
	fftOutput = fftwf_alloc_complex(FFT_BINS);

	// create fftw plan
	p = fftwf_plan_dft_r2c_1d(
		FFT_SIZE,					// size of 1d transform
		windowedInput.data(),		// pointer to input array
		fftOutput,					// pointer to output array
		FFTW_ESTIMATE				// unsigned flags
	);

	// start worker thread
	t= std::thread(&SpectrumAnalyzer::workerLoop, this);
}

SpectrumAnalyzer::~SpectrumAnalyzer() {
	// let worker loop know to stop
	stopRequested.store(true);

	if (t.joinable())
	{
		t.join();
	}

	// destroy plan
	fftwf_destroy_plan(p);
	fftwf_free(fftOutput);

	// delete buffers
	delete[] ringBufferData;
}

void SpectrumAnalyzer::loadData(std::span<const float> samples) noexcept {
	// grab the available space left in buffer
	ring_buffer_size_t available = PaUtil_GetRingBufferWriteAvailable(&ringBufferSpectrum);

	// take either full span or what's available
	ring_buffer_size_t count = std::min(
		static_cast<ring_buffer_size_t>(samples.size()),
		available
	);

	PaUtil_WriteRingBuffer(
		&ringBufferSpectrum,
		samples.data(),
		count
	);
}

void SpectrumAnalyzer::workerLoop() {
	// prepare fft float array
	std::array<float, HOP_SIZE> hop{};

	// pull from (nonempty) ringbuffer
	while (!stopRequested.load())
	{
		// grab available samples
		auto available = PaUtil_GetRingBufferReadAvailable(
			&ringBufferSpectrum
		);

		// process fft if enough samples (hop size), otherwise wait
		if (available >= HOP_SIZE)
		{
			PaUtil_ReadRingBuffer(
				&ringBufferSpectrum,
				hop.data(),
				HOP_SIZE
			);

			processFFT(hop);
		} else
		{
			std::this_thread::sleep_for(
				std::chrono::milliseconds(1)
			);
		}
	}
}

void SpectrumAnalyzer::processFFT(std::array<float, HOP_SIZE>& hop) {
	// move previous second half into first half
	std::copy(
		window.begin() + HOP_SIZE,
		window.end(),
		window.begin()
	);
	
	// replace window's last half with hop
	std::copy(
		hop.begin(),
		hop.end(),
		window.begin() + HOP_SIZE
	);

	// now multiply window by hanning window
	for (std::size_t i = 0; i < FFT_SIZE; i++)
	{
		windowedInput[i] = window[i] * hannWindow[i];
	}

	// now that we have the proper window, we can calculate the fft
	fftwf_execute(p);

	// build new local graph
	std::array<float, DISPLAY_BINS> localGraph{};

	for (std::size_t bar = 0; bar < DISPLAY_BINS; bar++)
	{
		std::size_t start = graphEdges[bar];
		std::size_t end = graphEdges[bar + 1];

		if (end <= start)
		{
			end = start + 1;
		}

		float maxDb = -120.0f;

		// calculate magnitude of each RC pair in freq range
		// record highest freq
		for (std::size_t k = start; k < end; k++)
		{
			float real = fftOutput[k][0];
			float imag = fftOutput[k][1];

			float magnitude = std::sqrt(real* real + imag * imag);

			// convert magnitude to signal amplitude
			float normalizedMagnitude = (2.0f * magnitude) / windowSum;

			float db = 20 * std::log10(normalizedMagnitude + 1e-12f);

			maxDb = std::max(maxDb, db);
		}

		// record maximum
		localGraph[bar] = maxDb;
	}

	// publish finished results with mutex
	{
		std::lock_guard<std::mutex> lock(graphMutex);
		graphBins = localGraph;
	}
}

std::array<float, SpectrumAnalyzer::DISPLAY_BINS> SpectrumAnalyzer::getGraph() const {
	// pull analyzer array (possibly wait for unlock)
	std::array<float, DISPLAY_BINS> snapshot;

	// lock and pull published graph
	{
		std::lock_guard<std::mutex> lock(graphMutex);
		snapshot = graphBins;
	}

	return snapshot;
}

void SpectrumAnalyzer::flushBuffers() {
	/*
	function used whenever a song is called to stop / restart
	*/

	// call buffer flush thing

	// zero fill window
}
