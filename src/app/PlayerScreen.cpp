#include "PlayerScreen.hpp"
#include "PlaybackControl.hpp"

#include <vector>
#include <string>

ftxui::Component PlayerScreen(AppState& state) {
    /*
    Defining the player screen and returning a ftxui::Component
    for App to receive
    */

    using namespace ftxui;
    
    // initialize playback bar
    auto playback_bar = PlaybackControl(state);
    
    auto renderer = Renderer(
        playback_bar, 
        [
            playback_bar,
            &state,
            albumBox = Box{}
        ]() mutable {
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

        auto& art = state.album_art;

        // checking for new song playing
        int width = albumBox.x_max - albumBox.x_min + 1;
        int height = albumBox.y_max - albumBox.y_min + 1;
        
        const std::string& currentPath = playing_track->getFilePath();

        bool trackChanged = art.file_path != currentPath;

        // check to see if track changed. then capture art bit stream
        if (trackChanged)
        {
            art.pixels.clear();

            art.source_width = 0;
            art.source_height = 0;

            art.loaded =
                playing_track->loadAlbumArt(
                    art.pixels,
                    art.source_width,
                    art.source_height
                );

            art.file_path = currentPath;

            art.rendered_height = 0;
            art.rendered_width = 0;
        }

        // checking for resizing / rerendering
        bool sizeChanged = width != art.rendered_width || height != art.rendered_height;

        if (art.loaded && sizeChanged && width > 2 && height > 2)
        {
            art.ascii =
                playing_track->renderASCII(
                    art.pixels,
                    art.source_width,
                    art.source_height,
                    width,
                    height
                );

            art.rendered_width = width;
            art.rendered_height = height;
        }

        auto albumPane = vbox({
            text(art.ascii) | center,
        })
        | flex
        | reflect(albumBox);

        // fix soon
        return vbox({
            hbox({
                albumPane,
            }) | flex,

            separator(),

            playback_bar->Render()
                | size(HEIGHT, EQUAL, 4),
        }) 
        | border;
    });

    return renderer;
}
