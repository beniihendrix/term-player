#include "PlaybackControl.hpp"

#include <ftxui/component/component_options.hpp>

ftxui::Component PlaybackControl(AppState& state) {
    /*
    Defining the control bar for both library and player screen
    */

    using namespace ftxui;

    // creating the rewind, pause, and skip buttons
    auto skip_button = Button(">>", [&state] {
        // insert lambda function that controls
        // the skipping using the player class in state
    });

    auto pause_button = Button("||", [&state] {
        // insert lambda function that controls
        // the playing toggle using the player class in state
    });

    auto rewind_button = Button("<<", [&state] {
        // insert lambda function that controls
        // the rewind using the player class in state
    });

    auto volume_slider = Slider("Volume: ",
        &state.volume,  // volume
        0,              // min
        100,            // max
        5              // increment
    );

    // keeping the components together using a container (enables navigation)
    auto playback_bar = Container::Horizontal({
        rewind_button,
        pause_button,
        skip_button,
        volume_slider,
    });

    return Renderer(playback_bar, [
        rewind_button,
        pause_button,
        skip_button,
        volume_slider,
        &state
    ] {
        return hbox({
            rewind_button->Render(),
            text(" "),
            pause_button->Render(),
            text(" "),
            skip_button->Render(),

            filler(),

            text("Volume"),
            volume_slider->Render()
                | size(WIDTH, EQUAL, 20),
        });
    });
}