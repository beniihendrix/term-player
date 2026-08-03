#include "database.h"
#include "track.h"
#include <string>
#include <filesystem>
#include <iostream>
#include <taglib/fileref.h>
#include <taglib/tag.h>

namespace fs = std::filesystem;

DataBase::DataBase(std::string filePath){
	/*
	 * This is the constructor for the database
	 * TODO: Check if path exists
	 * TODO: Create / open sqlite database
	 * TODO: Initialize the table
	 * TODO: Run recursive directory iterator
	 * TODO: Print mp3 file counts and status
	 * TODO: Replace error returns with thrown exceptions
	 */

	// path check
	fs::path targetDir = filePath;

	if (!fs::exists(targetDir) || !fs::is_directory(targetDir))
	{
		std::cerr << "Error: Invalid directory path.\n";
	}

	// creating / opening sqlite database
	dbPath = "music_library.db";	// store path for destructor
	int rc = sqlite3_open("music_library.db", &db);

	if (rc != SQLITE_OK)
	{
		std::cerr << "Cannot open database: " << sqlite3_errmsg(db) << "\n";
	}

	std::cout << "Successfully opened / created 'music_library.db\n";

	// initialize table
	if (!initializeDatabase())
	{
		sqlite3_close(db);
		std::cerr << "Error: Could not initialize database";
	}

	std::cout << "Searching for mp3 files...\n";
	std::cout << "--------------------------\n";

	size_t insertedCount = 0;

	// initialize iterator
	try{
		auto it = fs::recursive_directory_iterator(targetDir, fs::directory_options::skip_permission_denied);

		for (const auto& entry : it)
		{
			if (entry.is_regular_file())
			{
				fs::path filePath = entry.path();

				// turn filepath lowercase and pull metadata
				std::string ext = filePath.extension().string();
				std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

				if (ext == ".mp3")
				{
					std::string fullPathStr = filePath.string();

					// pull metadata
					// TrackMetaData meta = extractMetadata(fullPathStr);
					Track track(fullPathStr);
					if (insertTrack(track))
					{
						insertedCount++;
					}
				}
			}
		}
	}
	catch (const fs::filesystem_error& e)
	{
		std::cerr << "Filesystem error: " << e.what() << "\n";
	}

	std::cout << "------------------\n";
	std::cout << "Indexed " << insertedCount << " mp3 file(s) into the database.\n";
}

std::vector<Track> DataBase::dbQuery(const std::string& search){
	std::vector<Track> results;

	// this statement executes a db query and returns a vector of tracks,
	// meant to be scrolled through and called by the viewer class.

	sqlite3_stmt* stmt = nullptr;

	// example: searching by artist (I should change this)
	std::string sql = "SELECT file_path, title, artist, album, track_number FROM tracks WHERE artist LIKE ?;";
	sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);

	// bind search term to query
	std::string searchTerm = "%" + search + "%";
	sqlite3_bind_text(stmt, 1, searchTerm.c_str(), -1, SQLITE_TRANSIENT);

	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		// extract data
		const unsigned char* pathText = sqlite3_column_text(stmt, 0);
		Track track(pathText ? reinterpret_cast<const char*>(pathText) : "");

		results.push_back(track);
	}

	sqlite3_finalize(stmt);

	return results;
}

bool DataBase::initializeDatabase(){
	char* errorMessage = nullptr;

	// SQL statement that defines the structure
	std::string sql = "CREATE TABLE IF NOT EXISTS tracks ("
		"id INTEGER PRIMARY KEY AUTOINCREMENT, "
		"file_path TEXT UNIQUE, "
		"title TEXT DEFAULT 'Unknown Title', "
		"artist TEXT DEFAULT 'Unknown Artist', "
		"album TEXT DEFAULT 'Unknown Album', "
		"track_number INTEGER DEFAULT 0);";

	// attempting table opening with sqlite3_exec
	int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errorMessage);

	if (rc != SQLITE_OK)
	{
		std::cerr << "SQL Error creating table: " << errorMessage << "\n";
		sqlite3_free(errorMessage);
		return false;
	}

	return true;
}

bool DataBase::insertTrack(const Track& track){
	sqlite3_stmt* stmt = nullptr;

	// creating our prompt with '?' placeholders
	std::string sql = "INSERT OR IGNORE INTO tracks (file_path, title, artist, album, track_number) "
		"VALUES (?, ?, ?, ?, ?);";

	// compiling text into binary for sqlite
	int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
	if (rc != SQLITE_OK)
	{
		std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << "\n";
		return false;
	}

	// now binding our values to the placeholders
	sqlite3_bind_text(stmt, 1, track.getFilePath().c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 2, track.getTitle().c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 3, track.getArtist().c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 4, track.getAlbum().c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_int(stmt, 5, track.getTrack_Num());

	// execute statement
	rc = sqlite3_step(stmt);

	// releasing stmt from memory
	sqlite3_finalize(stmt);

	return (rc == SQLITE_DONE);
}

DataBase::~DataBase(){
	// destroy everything
	if (db != nullptr)
	{
		sqlite3_close(db);
		db = nullptr;
		std::cout << "Database connection closed.\n";
	}

	// delete the .db file
	if (fs::exists(dbPath))
	{
		try{
			fs::remove(dbPath);
			std::cout << "Deleted temporary database file: " << dbPath << "\n";
		} catch (const fs::filesystem_error& e)
		{
			std::cerr << "Error deleting database file: " << e.what() << "\n";
		}
	}
}
