#include "PlayerScreen.hpp"
#include "PlaybackControl.hpp"

#include <algorithm>
#include <vector>

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

    auto renderer = Renderer(playback_bar, [playback_bar, &state, spectrum] {
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
            hbox({
                text(playing_track->printASCII()) 
                    | center
                    | flex,

                separator(),

                graph(spectrum)
                    | size(HEIGHT, EQUAL, 12)
                    | flex,
            }),
            separator(),
            text(playing_track->getTitle()),
            text(playing_track->getAlbum()),
            text(playing_track->getArtist()),

            separator(),
            playback_bar->Render(),
        }) 
        | border;
    });

    return renderer;
}