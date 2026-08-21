#include "PlayerScreen.hpp"

ftxui::Component PlayerScreen(AppState& state) {
    /*
    Defining the player screen and returning a ftxui::Component
    for App to receive
    */

    using namespace ftxui;
    

    auto renderer = Renderer([&state] {
            const Track& selected_track = state.search_result[state.currently_playing_index];
                return vbox({
                text(selected_track.printASCII()),
                separator(),
                text(selected_track.getTitle()),
                text(selected_track.getAlbum()),
                text(selected_track.getArtist()),
                }) | border;
    });

    return renderer;
}