#include "Database.hpp"
#include "app/AppState.hpp"

#include <mutex>

std::mutex mtx;

namespace fs = std::filesystem;

Database::Database() {

}

int Database::InitializeDatabase() {
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
		sqlite3_free(errorMessage);
		return 1;
	}
    
    return 0;   // all good
}

int Database::BuildDataBase(AppState& state, ftxui::ScreenInteractive& screen) {
    // set private members from appstate
    setParentDirectory(state.directory);
    state.files_found = 0;
    
    int rc = sqlite3_open(dbPath.string().c_str(), &db);

    if (rc != SQLITE_OK)
    {
        return 1;   // cannot open database
    }

    if (InitializeDatabase() > 0)
    {
        sqlite3_close(db);
        // could not initialize database
        return 1;
    }

    // initialize iterator
    try{
        auto it = fs::recursive_directory_iterator(parentDirectory, fs::directory_options::skip_permission_denied);

        for (const auto& entry : it)
        {
            if (entry.is_regular_file())
            {
                fs::path filePath = entry.path();

                // turn extension to lowercase to check
                std::string ext = filePath.extension().string();
                std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

                if (ext == ".mp3" || ext == ".m4a")
                {
                    std::string fullPathStr = filePath.string();

                    // pull metadata
                    Track track(fullPathStr);

                    if (InsertTrack(track))
                    {// let appstate know what file is being pulled
                        // add a scoped lock for thread-safeness
                        std::lock_guard<std::mutex> lock(mtx);

                        state.current_file = track.getFilePath();
                        
                        state.files_found++;
                        // now lock will unlock when it goes out of these brackets
                    }

                    // give screen a postevent to react to!
                    screen.PostEvent(ftxui::Event::Custom);
                }
            }
        }
    } catch (const fs::filesystem_error& e)
    {
        // file system error
        return 1;
    }

    // give appstate the entire database to display
    state.search_result = dbQuery("", 0, state);

    // add number of tracks for string for menu option
    // Take away later
    state.library_entries.clear();
    
    for (const Track& track : state.search_result)
    {
        state.library_entries.push_back(track.getTitle());
    }

    return 0;   // all good
}

bool Database::InsertTrack(const Track& track) {
    sqlite3_stmt* stmt = nullptr;

    // creating sql command with '?' placeholders
    std::string sql = "INSERT OR IGNORE INTO tracks (file_path, title, artist, album, track_number) "
		"VALUES (?, ?, ?, ?, ?);";

    // compiling text into binary for sqlite
    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
	if (rc != SQLITE_OK)
	{
		// std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << "\n";
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

    bool inserted = rc == SQLITE_DONE && sqlite3_changes(db) > 0;

    // releasing stmt from memory
	sqlite3_finalize(stmt);

	return inserted;
}

std::vector<Track> Database::dbQuery(const std::string& search, int searchType, AppState& state) {
    std::vector<Track> results;

    // this statement executes a db query and returns a vector of tracks,
	// meant to be scrolled through and called by the viewer class.

	sqlite3_stmt* stmt = nullptr;
	std::string sql = "";

	// determine what kind of search with switch statement
	switch (searchType)
	{
		case 0:	// artist
			sql = "SELECT file_path, title, artist, album, track_number FROM tracks WHERE artist LIKE ? ORDER BY album ASC, track_number ASC;";
			break;
		case 1: // album
			sql = "SELECT file_path, title, artist, album, track_number FROM tracks WHERE album LIKE ? ORDER BY track_number ASC;";
			break;
		case 2:	// song
			sql = "SELECT file_path, title, artist, album, track_number FROM tracks WHERE title LIKE ? ORDER BY artist ASC, album ASC, track_number ASC;";
			break;
		default: // fallback in case invalid int was passed
			sql = "SELECT file_path, title, artist, album, track_number FROM tracks WHERE artist LIKE ? ORDER BY album ASC, track_number ASC;";
			break;
	}

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

Database::~Database() {
    if (db != nullptr)
    {
        sqlite3_close(db);
    }

    // delete .db file
    if (fs::exists(dbPath))
    {
        try {
            fs::remove(dbPath);
        } catch (const fs::filesystem_error& e)
        {
            // error deleting file
        }
    }
}