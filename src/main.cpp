#include "app/App.hpp"

#include <ftxui/component/screen_interactive.hpp>

int main(){
	AppState state;

	auto screen = ftxui::ScreenInteractive::TerminalOutput();

	auto app = App(state, screen);

	screen.Loop(app);
	
	return 0;
}