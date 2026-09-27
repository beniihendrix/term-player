#include "track.hpp"
#include <iostream>

// taglib for pulling metadata
#include <taglib/fileref.h>
#include <taglib/tag.h>
#include <mpegfile.h>

// ffmpeg for pulling album art
extern "C" {
	#include <libavformat/avformat.h>
}

#include <id3v2tag.h>
#include <taglib/attachedpictureframe.h>

// stb_image for converting to ascii
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Track::Track(std::string filePath){
	this->filePath = filePath;
	filePathObject = filePath;

	// use taglib to find the audio info
	TagLib::FileRef f(filePath.c_str());

	// checking if file is openable
	if (!f.isNull() && f.tag())
	{
		TagLib::Tag* tag = f.tag();

		// converting to c string and adding to members
		title = tag->title().toCString(true);
		artist = tag->artist().toCString(true);
		album = tag->album().toCString(true);
		track_num = tag->track();
		year = tag->year();
	}

	if (!f.isNull() && f.audioProperties())
	{
		TagLib::AudioProperties* properties = f.audioProperties();
		sampleRate = properties->sampleRate();
		bitrate = properties->bitrate();
		channels = properties->channels();
		// length = properties->length();
	}
}

std::vector<unsigned char> Track::getAlbumArt() const {
	AVFormatContext* formatContext = nullptr;

	if (avformat_open_input(&formatContext, filePath.c_str(), nullptr, nullptr) < 0)
	{
		return {};
	}

	if (avformat_find_stream_info(formatContext,nullptr) < 0)
	{
		avformat_close_input(&formatContext);
		return {};
	}

	std::vector<unsigned char> imageData;

	for (unsigned int i = 0; i < formatContext->nb_streams; i++)
	{
		AVStream* stream = formatContext->streams[i];

		if (stream->disposition & AV_DISPOSITION_ATTACHED_PIC)
		{
			const AVPacket& picture = stream->attached_pic;

			imageData.assign(
				picture.data,
				picture.data + picture.size
			);

			break;
		}
	}

	avformat_close_input(&formatContext);
	return imageData;
}

std::string Track::printASCII(int maxWidth, int maxHeight) const{
	// extract cover art image data
	auto imageData = getAlbumArt();

	if (imageData.empty())
	{
		return "No album art image found.";
	}

	// now hand raw bytes to stb_image
	int width, height, originalChannels;

	unsigned char* pixels = stbi_load_from_memory(
			imageData.data(),
			static_cast<int>(imageData.size()),
			&width,
			&height,
			&originalChannels,
			1	// for turning to grayscale
	);

	if (!pixels)
	{
		// std::cerr << "Failed to decode image data.\n";
		return "Failed to decode image data.\n";
	}

	// turn to ascii
	const double charAspectRatio = 2.0;

	int targetWidth = maxWidth;

	int targetHeight =
		static_cast<int>(
			(static_cast<double>(height) / width) *
			targetWidth /
			charAspectRatio
		);

	// if target box is small rectangle
	if (targetHeight > maxHeight)
	{
		targetHeight = maxHeight;

		targetWidth =
			static_cast<int>(
				(static_cast<double>(width) / height) *
				targetHeight *
				charAspectRatio
			);
	}

	targetWidth =
		std::max(1, targetWidth);

	targetHeight =
		std::max(1, targetHeight);

	// ascii characters from dark to light
	const std::string asciiRamp = " .:-=+*#%@";
	std::string output_frame = "";

	for (int y = 0; y < targetHeight; y++)
	{
		for (int x = 0; x < targetWidth; x++)
		{
			// mapping to new plane
			int origX = x * width / targetWidth;
			int origY = y * height / targetHeight;

			unsigned char brightness = pixels[origY * width + origX];
			int rampIndex = (brightness * (asciiRamp.length() -1)) / 255;
			output_frame += asciiRamp[rampIndex];
		}
		output_frame += "\n";
	}

	stbi_image_free(pixels);

	return output_frame;
}

bool Track::loadAlbumArt(
	std::vector<unsigned char>& pixels, 
	int& width, 
	int& height) const 
{

	pixels.clear();
	width = 0;
	height = 0;
	
	AVFormatContext* formatContext = nullptr;

	if (avformat_open_input(&formatContext, filePath.c_str(), nullptr, nullptr) < 0)
	{
		return {};
	}

	if (avformat_find_stream_info(formatContext,nullptr) < 0)
	{
		avformat_close_input(&formatContext);
		return {};
	}

	for (unsigned int i = 0; i < formatContext->nb_streams; i++)
	{
		AVStream* stream = formatContext->streams[i];

		if (stream->disposition & AV_DISPOSITION_ATTACHED_PIC)
		{
			const AVPacket& picture = stream->attached_pic;

			int channels = 0;

			unsigned char* decoded = stbi_load_from_memory(
				picture.data,
				picture.size,
				&width,
				&height,
				&channels,
				1	// grayscale
			);

			std::size_t pixelCount = 
				static_cast<std::size_t>(width) *
				static_cast<std::size_t>(height);

			pixels.assign(
				decoded,
				decoded + pixelCount
			);

			stbi_image_free(decoded);

			avformat_close_input(&formatContext);

			return true;
		}
	}

	avformat_close_input(&formatContext);
	return false;
}


std::string Track::renderASCII(
	const std::vector<unsigned char>& pixels,
	int sourceWidth,
	int sourceHeight,
	int maxWidth,
	int maxHeight) const 
{

	if (pixels.empty() || 
		sourceWidth <= 0 ||
		sourceHeight <= 0 ||
		maxWidth <= 0 ||
		maxHeight < 0)
	{
		return "No Album Art";
	}

	const double charAspectRatio = 2.0;

	int targetWidth = maxWidth;

    int targetHeight =
        static_cast<int>(
            (
                static_cast<double>(sourceHeight) /
                sourceWidth
            )
            * targetWidth
            / charAspectRatio
        );

    // If width-based scaling makes it too tall,
    // scale based on height instead.
    if (targetHeight > maxHeight)
    {
        targetHeight = maxHeight;

        targetWidth =
            static_cast<int>(
                (
                    static_cast<double>(sourceWidth) /
                    sourceHeight
                )
                * targetHeight
                * charAspectRatio
            );
    }

    targetWidth =
        std::max(1, targetWidth);

    targetHeight =
        std::max(1, targetHeight);

    const std::string asciiRamp =
        " .:-=+*#%@";

    std::string output;

    output.reserve(
        (targetWidth + 1) *
        targetHeight
    );

    for (int y = 0; y < targetHeight; ++y)
    {
        int sourceY =
            y * sourceHeight /
            targetHeight;

        for (int x = 0; x < targetWidth; ++x)
        {
            int sourceX =
                x * sourceWidth /
                targetWidth;

            unsigned char brightness =
                pixels[
                    sourceY * sourceWidth +
                    sourceX
                ];

            std::size_t rampIndex =
                brightness *
                (asciiRamp.size() - 1) /
                255;

            output += asciiRamp[rampIndex];
        }

        output += '\n';
    }

    return output;
}

std::string Track::getExtension() const {
	return filePathObject.extension().string();
}

void Track::printFull() const {
	this->printASCII(50, 50);
	std::cout << "File Location: " << filePath << "\n";
	std::cout << "Title: " << title << "\n";
	std::cout << "Album: " << album << "\n";
	std::cout << "Artist: " << artist << "\n";
}
