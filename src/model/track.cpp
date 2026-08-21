#include "track.hpp"
#include <iostream>

// taglib for pulling metadata
#include <taglib/fileref.h>
#include <taglib/tag.h>
#include <mpegfile.h>
#include <id3v2tag.h>
#include <taglib/attachedpictureframe.h>

// stb_image for converting to ascii
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Track::Track(std::string filePath){
	this->filePath = filePath;

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

std::string Track::printASCII() const{
	// extract cover art image data
	TagLib::MPEG::File mpegFile(filePath.c_str());
	if (!mpegFile.isValid() || !mpegFile.ID3v2Tag())
	{
		// std::cerr << "Could not read audio file or metadata.\n";
		return "Could not display file";
	}

	TagLib::ID3v2::Tag* id3v2 = mpegFile.ID3v2Tag();
	TagLib::ID3v2::FrameList frames = id3v2->frameList("APIC");

	if (frames.isEmpty())
	{
		// std::cout << "No album art image found in this track.\n";
		return "No album art image found in this track.\n";
	}

	auto* frame = static_cast<TagLib::ID3v2::AttachedPictureFrame*>(frames.front());
	TagLib::ByteVector imgData = frame->picture();

	if (imgData.isEmpty())
	{
		// std::cout << "Album art buffer is empty.\n";
		return "Album art buffer is empty.\n";
	}

	// now hand raw bytes to stb_image
	int width, height, originalChannels;

	unsigned char* pixels = stbi_load_from_memory(
			reinterpret_cast<const unsigned char*>(imgData.data()),
			imgData.size(),
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
	const int targetWidth = 60; // semi arbitrary number
	const double charAspectRatio = 2.0;	// characters are tall
	int targetHeight = static_cast<int>((static_cast<double>(height) / width) * targetWidth / charAspectRatio);

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

void Track::printFull() const {
	this->printASCII();
	std::cout << "File Location: " << filePath << "\n";
	std::cout << "Title: " << title << "\n";
	std::cout << "Album: " << album << "\n";
	std::cout << "Artist: " << artist << "\n";
}

