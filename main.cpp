#include <iostream>
#include <string>
#include "database.h"
#include "viewer.h"
#include "player.h"

void clearScreen() {
	std::cout << "\033[2J\033[H" << std::flush;
}

int main(){

	clearScreen();
	// receiving filepath from user
	std::string usrInput;
	std::cout << "Enter a filepath: ";

	std::getline(std::cin, usrInput);

	std::cout << "Initializing database...\n";

	std::cout << "Initializing player\n";

	Player player;

	auto musicDb = std::make_unique<DataBase>(usrInput);

	while(usrInput != "0"){
		std::cout << "Database has been generated." << std::endl;
		std::cout << "Please enter a number to continue:" << std::endl;
		std::cout << "1: Search by Artist" << std::endl;
		std::cout << "2: Scroll by Album" << std::endl;
		std::cout << "3: Scroll by Song" << std::endl;
		std::cout << "0: Exit" << std::endl;

		std::getline(std::cin, usrInput);

		clearScreen();	// clear screen to avoid clutter
		

		if (usrInput == "1")
		{
			std::cout << "Please search by artist:\n";
			std::getline(std::cin, usrInput);

			std::vector<Track> results = musicDb->dbQuery(usrInput);
			// test by printout out items of vector
			for (const auto& element : results)
			{
				std::cout << element << "\n";
			}

			// viewer implementation
			Viewer artistView(results);

			while (usrInput != "0")
			{
				clearScreen();
				std::cout << "Type 0 to exit fully\n";
				std::cout << "Type 'up' to ascend in playlist, 'down' to descend in playlist\n";

				if (usrInput == "up")
				{
					artistView.nextTrack();
				} else if (usrInput == "down")
				{
					artistView.previousTrack();
				} else if (usrInput == "play")
				{
					std::cout << "Now Playing: " << artistView.getCurrentFilePath() << std::endl;
					player.play(artistView.getCurrentFilePath());
				} else if (usrInput == "stop")
				{
					player.stop();
				} else {
					artistView.displayCurrentTrack();
				}

				std::getline(std::cin, usrInput);
			}

			player.stop();

		} else if (usrInput == "2")
		{
			std::cout << "You are scrolling by Album\n";
		} else if (usrInput == "3")
		{
			std::cout << "You are scrolling by Song.\n";
		}
	}

	std::cout << "Deleting Database and exiting gracefully..." << std::endl;

	clearScreen();

	return 0;
}
