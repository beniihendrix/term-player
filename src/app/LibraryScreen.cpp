#include "LibraryScreen.hpp"
#include "PlaybackControl.hpp"

#include <ftxui/component/component_options.hpp>

ftxui::Component LibraryScreen(AppState& state) {
    /*
    Defining the loading screen and returning a ftxui::Component
    for App to receive
    */

    using namespace ftxui;

    MenuOption option;

    // creating menu customization for rows with multiple elements
    option.entries_option.transform =
        [&state](EntryState entry) -> Element {

            if (entry.index >= state.search_result.size())
            {
                return text("");
            }

            const Track& track = state.search_result[entry.index];

            Element row = hbox({
                // display track number
                text(std::to_string(track.getTrack_Num()))
                    | size(WIDTH, EQUAL, 5),
                // display title
                text(track.getTitle())
                    | size(WIDTH, EQUAL, 30),
                // display album
                text(track.getAlbum())
                    | size(WIDTH, EQUAL, 25),
                // display artist
                text(track.getArtist())
                    | size(WIDTH, EQUAL, 25),
            });

            if (entry.active)
            {
                row = row | inverted;
            }

            return row;
        };

    // adding .on_enter functionality
    option.on_enter = [&state] {
        if (state.search_result.empty())
        {
            return;
        }

        state.controller.playIndex(
            static_cast<std::size_t>(state.selected_track)
        );
    };

    auto song_menu = Menu(
        &state.library_entries,
        &state.selected_track,
        option
    );

    // adding playback bar
    auto playback_bar = PlaybackControl(state);

    auto screen_container = Container::Vertical({
        song_menu,
        playback_bar
    });

    return Renderer(screen_container, [song_menu, playback_bar, &state] {
        const Track& selected_track = state.search_result[state.selected_track];
        // pulling the address of the selected track
        
        auto header = hbox({
            text("#")       | size(WIDTH, EQUAL, 5),
            text("Title")   | size(WIDTH, EQUAL, 30),
            text("Album")   | size(WIDTH, EQUAL, 25),
            text("Artist")  | size(WIDTH, EQUAL, 25),
        });

        return vbox({
            header,
            separator(),
            hbox({
                song_menu->Render()
                | vscroll_indicator
                | frame
                | flex,
                
                separator(),

                text(selected_track.printASCII()) | center
            }) | flex,
            separator(),
            playback_bar->Render(),
        }) | border;
    });
}