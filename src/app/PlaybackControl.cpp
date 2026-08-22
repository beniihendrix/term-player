#include "PlaybackControl.hpp"

#include <ftxui/component/component_options.hpp>

ftxui::Component PlaybackControl(AppState& state) {
    /*
    Defining the control bar for both library and player screen
    */

    using namespace ftxui;

    // creating the rewind, pause, and skip buttons
    auto skip_button = Button(">>", [&state] {
        state.controller.skip();
    });

    auto pause_button = Button("||", [&state] {
        state.controller.togglePause();
    });

    auto rewind_button = Button("<<", [&state] {
        state.controller.rewind();
    });

    // add a slider with a function that updates the controller
    SliderOption<float> volume_option;

    volume_option.value = &state.volume;
    volume_option.min = 0.0f;
    volume_option.max = 1.0f;
    volume_option.increment = 0.05f;

    volume_option.on_change = [&state] {
        state.controller.setVolume(state.volume);
    };

    auto volume_slider = Slider(volume_option);

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

            text("Volume: " + std::to_string(state.volume)),
            volume_slider->Render()
                | size(WIDTH, EQUAL, 20),
        });
    });
}