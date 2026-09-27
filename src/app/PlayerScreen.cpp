#include "PlayerScreen.hpp"
#include "PlaybackControl.hpp"

#include <algorithm>
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

    // create graph
    auto spectrum = [&state](int width, int height) {
        std::vector<int> output(width, 0);

        // expose the analyzer output
        auto bins = state.controller.getGraph();

        if (bins.empty() || width <= 0 || height <= 0)
        {
            return output;
        }

        for (int x = 0; x < width; x++)
        {
            // map terminal column per spectrum bin
            std::size_t bin =
                static_cast<std::size_t>(
                    static_cast<double>(x) /
                    static_cast<double>(width) *
                    bins.size()
                );
            
            bin = std::min(
                bin,
                bins.size() - 1
            );

            float value = bins[bin];

            constexpr float minDb = -80.0f;
            constexpr float maxDb = 0.0f;

            float normalized =
                (value - minDb) /
                (maxDb - minDb);

            normalized = std::clamp(
                normalized,
                0.0f,
                1.0f
            );

            output[x] = static_cast<int>(
                normalized * height
            );
        }

        return output;
    };


    auto renderer = Renderer(
        playback_bar, 
        [
            playback_bar,
            &state,
            spectrum,
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

                separator(),

                graph(spectrum)
                    | flex,
            }) | flex,

            separator(),

            playback_bar->Render()
                | size(HEIGHT, EQUAL, 4),
        }) 
        | border;
    });

    return renderer;
}