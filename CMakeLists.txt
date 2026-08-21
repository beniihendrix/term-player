cmake_minimum_required(VERSION 3.16)
project(PlayerUI LANGUAGES C CXX)

# force cpp17 support
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# export compilation for vim lsp
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# find sqlite
find_package(SQLite3 REQUIRED)

# find taglib
find_package(PkgConfig REQUIRED)
pkg_check_modules(TAGLIB REQUIRED IMPORTED_TARGET taglib)

# adding fetchcontent for ftxui library
include(FetchContent)
FetchContent_Declare(ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
  GIT_TAG main # or a specific version like v7.0.3
)
FetchContent_MakeAvailable(ftxui)

# use pkgconfig to find c libraries
pkg_check_modules(AVCODEC REQUIRED IMPORTED_TARGET libavcodec)
pkg_check_modules(AVFORMAT REQUIRED IMPORTED_TARGET libavformat)
pkg_check_modules(AVUTIL REQUIRED IMPORTED_TARGET libavutil)
pkg_check_modules(SWRESAMPLE REQUIRED IMPORTED_TARGET libswresample)

# check for portaudio
pkg_check_modules(PORTAUDIO REQUIRED IMPORTED_TARGET portaudio-2.0)

add_executable(PlayerUI
	src/main.cpp

	src/app/App.cpp
	src/app/FolderFindScreen.cpp
	src/app/LoadingScreen.cpp
	src/app/LibraryScreen.cpp
	src/app/PlayerScreen.cpp

	src/database/Database.cpp

	src/model/track.cpp

	src/audio/player.cpp
	src/audio/pa_ringbuffer.c
)

target_include_directories(PlayerUI PRIVATE src)

target_link_libraries(PlayerUI PRIVATE
	SQLite3::SQLite3
	PkgConfig::TAGLIB
	PkgConfig::AVCODEC
	PkgConfig::AVFORMAT
	PkgConfig::AVUTIL
	PkgConfig::SWRESAMPLE
	PkgConfig::PORTAUDIO
	ftxui::ftxui
)
