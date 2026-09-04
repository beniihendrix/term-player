#include "app/App.hpp"

#include <atomic>
#include <thread>
#include <chrono>

#include <ftxui/component/screen_interactive.hpp>

int main(){
	AppState state;

	auto screen = ftxui::ScreenInteractive::Fullscreen();	// avoids redraw shifts

	auto app = App(state, screen);

	// added a running check to see if a song reached end of file
	std::atomic<bool> running{true};

	std::thread update_thread([&] {
		using namespace std::chrono_literals;

		while (running.load())
		{
			std::this_thread::sleep_for(100ms);

			if (!running.load())
			{
				break;
			}

			screen.Post([&] {
				state.controller.update();
			});

			// for ftxui to redraw
			screen.PostEvent(ftxui::Event::Custom);
		}
	});

	screen.Loop(app);

	running.store(false);
	update_thread.join();
	
	return 0;
}
