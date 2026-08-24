#include "FolderFindScreen.hpp"

#include <filesystem>
#include <thread>

ftxui::Component FolderFindScreen(AppState& state, ftxui::ScreenInteractive& screen) {
    /*
    Defining the folder find screen and returning a ftxui::Component
    for App to receive
    */

    using namespace ftxui;
    namespace fs = std::filesystem;

    // creating string input slot
    Component directory_input = Input(&state.directory, "Directory Input");
    // adding folder path check button
    Component path_check = Button("Build Database", [&state, &screen] {
        if (state.directory.empty())
        {
            state.validation_message = "Path is Empty";
            return;
        }

        try
        {
            fs::path p(state.directory);
            if (fs::exists(p) && fs::is_directory(p))
            {
                state.validation_message = "Path is is valid";

                // launch database building to switch window and update progress
                std::thread([&state, &screen] {
                    state.SetScreen(AppState::Screen::Loading);
                    screen.PostEvent(Event::Custom);

                    int result = state.database.BuildDataBase(state, screen);

                    // if successful, go to library screen!
                    if (result == 0)
                    {
                        state.SetScreen(AppState::Screen::Library);
                    } else {
                        // give an error screen idk
                    }

                    screen.PostEvent(ftxui::Event::Custom);
                }).detach();
            } else if (fs::exists(p))
            {
                state.validation_message = "Found, but not folder";
            } else
            {
                state.validation_message = "Directory does not exist";
            }
        } catch (const fs::filesystem_error& e)
        {
            state.validation_message = "Invalid path syntax or system error";
        }
    });

    // TODO: adding a quit button

    auto container = Container::Vertical({
        directory_input,
        path_check,
    });

    auto renderer = Renderer(container,
        [container, directory_input, path_check, &state] {
        return vbox({
            text("Please type in a folder path to create your database."),
            separator(),
            vbox({
                hbox(text(" Path: "), directory_input->Render()),
                hbox(text(" Result: "), text(state.validation_message)),
            }),
            separator(),
            path_check->Render(),
            separator(),
            text(state.mainScreenArt) | center,
        }) 
        | border
        | flex;
    });

    return renderer;
}