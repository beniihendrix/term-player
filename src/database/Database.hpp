#pragma once

#include "model/track.hpp"

#include <string>
#include <vector>
#include <filesystem>
#include <sqlite3.h>
#include <ftxui/component/screen_interactive.hpp>

struct AppState;

class Database {
public:
    Database();
    ~Database();

    void setParentDirectory(const std::string& stringPath) {
        parentDirectory = std::filesystem::path(stringPath);
        dbPath = parentDirectory / "music.db";
    }

    int InitializeDatabase();
    int BuildDataBase(AppState& state, ftxui::ScreenInteractive& screen);
    bool InsertTrack(const Track& track);

    std::vector<Track> dbQuery(const std::string& search, int searchType, AppState& state);
private:
    std::filesystem::path parentDirectory;
    std::filesystem::path dbPath;
    sqlite3* db = nullptr;
};