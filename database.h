/*
 * This file creates the database class
 * It's going to use sqlite queries to search for songs
 *
 */

#include <string>
#include <filesystem>
#include <sqlite3.h>
#include <vector>
#include "track.h"

class DataBase {
	public:
		DataBase(std::string filePath);
		std::vector<Track> dbQuery(const std::string& search);
		bool initializeDatabase();
		bool insertTrack(const Track& track);
		~DataBase();
	private:
		sqlite3* db = nullptr;
		std::filesystem::path folderPath;
		std::filesystem::path dbPath;
};

