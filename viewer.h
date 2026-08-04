#pragma once

#include "track.h"
#include <string>

class Viewer {
private:
	std::vector<Track> currentPlaylist;
	int current_index = 0;
public:
	Viewer(const std::vector<Track>& searchResults);
	~Viewer() = default;
	int getCurrent_Index() {return current_index;}
	std::string getCurrentFilePath();
	void nextTrack();
	void previousTrack();
	void displayCurrentTrack();
};
