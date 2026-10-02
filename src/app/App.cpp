#include "App.hpp"
#include "FolderFindScreen.hpp"
#include "LoadingScreen.hpp"
#include "LibraryScreen.hpp"
#include "PlayerScreen.hpp"
#include "SpectrogramScreen.hpp"

ftxui::Component App(AppState& state, ftxui::ScreenInteractive& screen) {
    using namespace ftxui;
    
    auto folder_find = FolderFindScreen(state, screen);
    auto loading = LoadingScreen(state);
    auto library = LibraryScreen(state);
    auto player = PlayerScreen(state);
    auto spectrogram = SpectrogramScreen(state);

    auto screens = ftxui::Container::Tab(
        {
            folder_find,
            loading,
            library,
            player,
	    spectrogram,
        },
        &state.screen_index
    );

    auto quit_button = Button("Quit", screen.ExitLoopClosure());

    auto container = Container::Vertical({
        screens,
        quit_button,
    });

    auto app = Renderer(container, [screens, quit_button] {
        return vbox({
            screens->Render() | flex,
            separator(),
            quit_button->Render(),
        });
    });

    // adding tab event to switch between screens
    app |= CatchEvent([&](Event event) {
        if (event == Event::Tab)
        {
            if (state.GetScreen() == AppState::Screen::Library)
            {
                state.SetScreen(AppState::Screen::Player);
            } else if (state.GetScreen() == AppState::Screen::Player)
            {
                state.SetScreen(AppState::Screen::Spectrogram);
            } else if (state.GetScreen() == AppState::Screen::Spectrogram)
	    {
	    	state.SetScreen(AppState::Screen::Library);
	    }

            return true;
        }

        return false;
    });

    return app;
}
