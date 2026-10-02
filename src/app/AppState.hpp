/*
This struct holds the up-to-date info for the App function
to return the correct screen
*/
#pragma once

#include "database/Database.hpp"
#include "model/track.hpp"
#include "audio/PlaybackController.hpp"

#include <string>
#include <vector>

struct AppState {
    enum class Screen {
        FolderFind = 0,
        Loading,
        Library,
        Player,
	Spectrogram
    };

    int screen_index = static_cast<int>(Screen::FolderFind);

    // Folder Find Variables
    std::string directory;
    std::string validation_message = "Status: Awaiting Validation";

    // Database instance for project
    Database database;
    std::vector<Track> search_result;
    std::vector<std::string> library_entries;

    // Loading Screen Updates
    float loading_progress = 0.0f;
    std::string current_file;
    int files_found = 0;
    std::string loading_status = "Opening Database...";

    // trying to add new playback controller class
    PlaybackController controller;
    int selected_track = 0; // only UI needs to know
    float volume = 1.0f;    // just for UI

    // album art data
    struct AlbumArt 
    {
        std::vector<unsigned char> pixels;

        int source_width = 0;
        int source_height = 0;

        std::string file_path;

        std::string ascii = "No Album to Display";

        int rendered_width = 0;
        int rendered_height = 0;

        bool loaded = false;
    };

    AlbumArt album_art;

    void SetScreen(Screen screen) {
        screen_index = static_cast<int>(screen);
    }

    Screen GetScreen() const {
        return static_cast<Screen>(screen_index);
    }

    std::string mainScreenArt = R"(   
   _____              .___       ___.               __________              .__       .__        
  /     \ _____     __| _/____   \_ |__ ___.__. /\  \______   \ ____   ____ |__| ____ |__| ____  
 /  \ /  \\__  \   / __ |/ __ \   | __ <   |  | \/   |    |  _// __ \ /    \|  |/ ___\|  |/  _ \ 
/    Y    \/ __ \_/ /_/ \  ___/   | \_\ \___  | /\   |    |   \  ___/|   |  \  \  \___|  (  <_> )
\____|__  (____  /\____ |\___  >  |___  / ____| \/   |______  /\___  >___|  /__|\___  >__|\____/ 
        \/     \/      \/    \/       \/\/                  \/     \/     \/        \/           
    )";
};
