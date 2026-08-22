#include "PlayerScreen.hpp"
#include "PlaybackControl.hpp"

ftxui::Component PlayerScreen(AppState& state) {
    /*
    Defining the player screen and returning a ftxui::Component
    for App to receive
    */

    using namespace ftxui;
    
    auto playback_bar = PlaybackControl(state);

    auto renderer = Renderer(playback_bar, [playback_bar, &state] {
        // using the new call to get currently playing track
        const Track* playing_track = state.controller.getPlayingTrack();

        if (!playing_track)
        {
            return vbox({
                filler(),
                text("Nothing Playing") | center,
                filler(),
                separator(),
                playback_bar->Render(),
            }) | border;
        }

        // fix soon
        return vbox({
            text(playing_track->printASCII()) | center,
            separator(),
            text(playing_track->getTitle()),
            text(playing_track->getAlbum()),
            text(playing_track->getArtist()),

            separator(),
            playback_bar->Render(),
        }) | border;
    });

    return renderer;
}