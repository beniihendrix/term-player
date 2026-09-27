#pragma once

#include <string>
#include <iostream>
#include <filesystem>
#include <vector>

class Track{
	public:
		Track(std::string filePath);
		~Track() = default;

		// my getters
		std::string getFilePath() const {return filePath; }
		std::string getTitle() const {return title; }
		std::string getArtist() const {return artist; }
		std::string getAlbum() const {return album; }
		int getTrack_Num() const {return track_num; }
		int getYear() const {return year; }
		float getSampleRate() const {return sampleRate; }
		int getBitRate() const {return bitrate; }
		std::string getExtension() const;
		
		// ascii generation
		bool loadAlbumArt(
			std::vector<unsigned char>& pixels, 
			int& width, 
			int& height
		) const;
		std::string renderASCII(
			const std::vector<unsigned char>& pixels,
			int sourceWidth,
			int sourceHeight,
			int maxWidth,
			int maxHeight
		) const;
		std::vector<unsigned char> getAlbumArt() const;
		std::string printASCII(int maxWidth, int maxHeight) const;

		// operator overloader test for display track
		friend std::ostream& operator<<(std::ostream& os, const Track& track){
			os << track.getTitle() << " - " << track.getArtist() << " [" << track.getAlbum() << "]";
			return os;
		}

		// printing full data
		void printFull() const;	// const because it doesn't change any members
	private:
		std::string filePath;
		std::filesystem::path filePathObject;
		std::string title = "Unknown Title";
		std::string artist = "Unkown Artist";
		std::string album = "Unknown Album";
		int track_num = 0;
		int year = 0;
		int sampleRate = 44100;
		int bitrate = 128;
		int channels = 2;
};
