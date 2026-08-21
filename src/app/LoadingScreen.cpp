#include "LoadingScreen.hpp"

#include <filesystem>

ftxui::Component LoadingScreen(AppState& state) {
    /*
    Defining the loading screen and returning a ftxui::Component
    for App to receive
    */

    using namespace ftxui;

    return Renderer([&state] {
        // display current file name, not path
        std::filesystem::path path(state.current_file);
        
        return window(
            text(" Building Music Database "),
            vbox({
                text(state.loading_status),
                text("Files found: " +
                     std::to_string(state.files_found)),
                text("Current: " + path.filename().string()),
                separator(),
                gauge(state.loading_progress),
            })
        ) 
        | size(WIDTH, EQUAL, 80)
        | size(HEIGHT, EQUAL, 12)
        | center;
    });
}