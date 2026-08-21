#pragma once

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include "AppState.hpp"

ftxui::Component App(AppState& state, ftxui::ScreenInteractive& screen);