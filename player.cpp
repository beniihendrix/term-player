#include "player.h"
#include <iostream>
#include <string>
#include <thread>

extern "C" {
    #include <libavformat/avformat.h>
    #include <libavcodec/avcodec.h>
    #include <libavutil/avutil.h>
    #include <libswresample/swresample.h>
}

Player::Player(){
    // allocate ring buffer memory
    unsigned int numFrames = 8192; // power of 2
    unsigned int bytesPerFrame = 2 * sizeof(float);
    ringBufferData = new char[numFrames * bytesPerFrame];
    PaUtil_InitializeRingBuffer(&ringBuffer, bytesPerFrame, numFrames, ringBufferData);

    // initialize audio hardware
    Pa_Initialize();
}

Player::~Player(){
    stop();
    delete[] ringBufferData;
    Pa_Terminate();
}

void Player::play(const std::string& path){
    if (isPlaying)
    {
        stop(); // stop playing last song
    }

    filePath = path;
    isPlaying = true;

    // open PA stream
    Pa_OpenDefaultStream(
        &stream,
        0,
        2,
        paFloat32,
        44100,
        256,
        &Player::portAudioCallback,
        this
    );

    Pa_StartStream(stream);

    // launch FFmpeg decoding on background thread to feed ring buffer
    decodeThread = std::thread(&Player::decodeLoop, this);
    
    return;
}

void Player::stop(){
    if (!isPlaying) return;
    isPlaying = false;

    if (decodeThread.joinable())
    {
        decodeThread.join();
    }

    if (stream)
    {
        Pa_StopStream(stream);
        Pa_CloseStream(stream);
        stream = nullptr;   // clear stream
    }

    return;
}

int Player::portAudioCallback(
        const void* inputBuffer, void* outputBuffer,
        unsigned long framesPerBuffer,
        const PaStreamCallbackTimeInfo* timeInfo,
        PaStreamCallbackFlags statusFlags,
        void* userData
    ){
        Player* playerInstance = static_cast<Player*>(userData);

        // route data to audio processing function

        return playerInstance->processAudio(static_cast<float*>(outputBuffer), framesPerBuffer);
    }

int Player::processAudio(float* output, unsigned long framesPerBuffer){
    // TODO: Pull raw PCM blocks from ring buffer here
    
    unsigned long framesRead = 0;

    ring_buffer_size_t elementsAvailable = PaUtil_GetRingBufferReadAvailable(&ringBuffer);

    if (elementsAvailable >= framesPerBuffer)
    {
        PaUtil_ReadRingBuffer(&ringBuffer, output, framesPerBuffer);
        framesRead = framesPerBuffer;
    } else
    {
        if (elementsAvailable > 0)
        {
            PaUtil_ReadRingBuffer(&ringBuffer, output, elementsAvailable);
        }

        unsigned long floatsWritten = elementsAvailable * 2;
        unsigned long totalFloatsNeeded = framesPerBuffer * 2;

        std::fill(output + floatsWritten, output + totalFloatsNeeded, 0.0f);
    }
    
    return paContinue;
}

void Player::decodeLoop(){
    // create a format context and open audio file using path
    AVFormatContext* format_ctx{nullptr};

    int ret = avformat_open_input(&format_ctx, 
        filePath.c_str(), nullptr, nullptr);

    if (ret < 0)
    {
        std::cerr << "Could not create format context." << std::endl;
        return;
    }

    ret = avformat_find_stream_info(format_ctx, nullptr);
    if (ret < 0)
    {
        std::cerr << "Unable to find stream info." << std::endl;
        return;
    }

    // need to parse through file to find the index of audio stream
    int index = av_find_best_stream(format_ctx, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
    if (index < 0)
    {
        std::cerr << "No audio stream inside of this file." << std::endl;
        return;
    }

    // making a new AVStream* struct just for brevity
    AVStream* stream = format_ctx->streams[index];

    // creating audio codec
    const AVCodec* decoder = avcodec_find_decoder(stream->codecpar->codec_id);
    if (!decoder)
    {
        std::cerr << "No decoder found." << std::endl;
        return;
    }

    // codec needs a context now
    AVCodecContext* codec_ctx{avcodec_alloc_context3(decoder)};

    avcodec_parameters_to_context(codec_ctx, stream->codecpar);

    // open decoder
    ret = avcodec_open2(codec_ctx, decoder, nullptr);
    if (ret < 0)
    {
        std::cerr << "Could not open decoder." << std::endl;
        return;
    }

    // ACTUALLY DECODE AUDIO
    // pull compressed audio packets from stream and give to decoder
    
    // allocate memory for compressed packets and uncompressed frames
    AVPacket* packet = av_packet_alloc();
    AVFrame* frame = av_frame_alloc();

    // making a resample context for interleaved format and such
    SwrContext* resampler = NULL;

    ret = swr_alloc_set_opts2(&resampler, 
        &stream->codecpar->ch_layout,
        AV_SAMPLE_FMT_FLT,
        stream->codecpar->sample_rate,
        &stream->codecpar->ch_layout,
        (AVSampleFormat)stream->codecpar->format,
        stream->codecpar->sample_rate,
        0,
        nullptr);
    
    if (ret < 0)
    {
        std::cerr << "Could not open resampler!" << std::endl;
        return;
    }

    // actual decode loop
    while ((av_read_frame(format_ctx, packet) == 0) && isPlaying)
    {
        // open frame and send to decoder
        if (packet->stream_index != index) 
        {
            av_packet_unref(packet);    // no data leaks
            continue;
        }
        ret = avcodec_send_packet(codec_ctx, packet);
        if (ret < 0)
        {
            // AVERROR(EAGAIN) --> send packet again
            if (ret != AVERROR(EAGAIN))
            {
                std::cerr << "Some decoding error occured." << std::endl;
            }
        }
        av_packet_unref(packet);    // no data leaks after sending

        while ((ret = avcodec_receive_frame(codec_ctx, frame)) == 0)
        {
            // doing a while loop since we may receive multiple frames
            // we resample to interleaved for portaudio
            AVFrame* resampled_frame = av_frame_alloc();
            resampled_frame->sample_rate = frame->sample_rate;
            av_channel_layout_copy(&resampled_frame->ch_layout, &frame->ch_layout);
            resampled_frame->format = AV_SAMPLE_FMT_FLT;

            ret = swr_convert_frame(resampler, resampled_frame, frame);
            if (ret < 0)
            {
                std::cout << "could not convert frame!" << std::endl;
            }
            av_frame_unref(frame); // frame freed

            // now write to buffer
            int decoded_frames = resampled_frame->nb_samples;

            // checking for room in buffer; if none, wait
            while (isPlaying && PaUtil_GetRingBufferWriteAvailable(&ringBuffer) < decoded_frames)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(2));
            }

            if (isPlaying)
            {
                PaUtil_WriteRingBuffer(&ringBuffer, resampled_frame->data[0], decoded_frames);
            }

            av_frame_free(&resampled_frame);
        }
    }

    // now closing up shop after we've been told to stop playing or run out of data
    avformat_close_input(&format_ctx);
    av_frame_free(&frame);
    av_packet_free(&packet);
    avcodec_free_context(&codec_ctx);
    swr_free(&resampler);

    return;
}