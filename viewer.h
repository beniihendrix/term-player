#pragma once

#include "track.h"

class Viewer {
private:
	std::vector<Track> currentPlaylist;
	int current_index = 0;
public:
	Viewer(const std::vector<Track>& searchResults);
	~Viewer() = default;
	void nextTrack();
	void previousTrack();
	void displayCurrentTrack();
};
