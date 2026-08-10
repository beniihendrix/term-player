#include "viewer.h"
#include "track.h"
#include <iostream>

Viewer::Viewer(const std::vector<Track>& searchResults){
	currentPlaylist = searchResults;
	current_index = 0;
	displayCurrentTrack();
}

void Viewer::displayCurrentTrack(){
	if (currentPlaylist.empty())
	{
		std::cout << "No tracks found\n";
		return;
	}

	const Track& currentTrack = currentPlaylist[current_index];
	std::cout << "\n-=- Current Viewing (Item " << (current_index + 1) << " of " << currentPlaylist.size() << ") -=-\n";
	currentTrack.printFull();
}

void Viewer::nextTrack(){
	if (currentPlaylist.empty()) return;

	if (current_index < currentPlaylist.size() - 1)
	{
		current_index++;
		displayCurrentTrack();
	} else {
		std::cout << "You are at the end of the list.\n";
	}
}

std::string Viewer::getCurrentFilePath(){
	const Track& currentTrack = currentPlaylist[current_index];
	return currentTrack.getFilePath();
}

void Viewer::previousTrack(){
	if (currentPlaylist.empty()) return;

	if (current_index > 0)
	{
		current_index--;
		displayCurrentTrack();
	} else {
		std::cout << "You are already at the beginning of the list.\n";
	}
}
