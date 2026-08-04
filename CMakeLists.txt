cmake_minimum_required(VERSION 3.16)
project(TerminalView LANGUAGES C CXX)

# force cpp17 support
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# export compilation for vim lsp
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# find sqlite
find_package(SQLite3 REQUIRED)

# find taglib
find_package(PkgConfig REQUIRED)
pkg_check_modules(TAGLIB REQUIRED IMPORTED_TARGET taglib)

# use pkgconfig to find c libraries
pkg_check_modules(AVCODEC REQUIRED IMPORTED_TARGET libavcodec)
pkg_check_modules(AVFORMAT REQUIRED IMPORTED_TARGET libavformat)
pkg_check_modules(AVUTIL REQUIRED IMPORTED_TARGET libavutil)
pkg_check_modules(SWRESAMPLE REQUIRED IMPORTED_TARGET libswresample)

# check for portaudio
pkg_check_modules(PORTAUDIO REQUIRED IMPORTED_TARGET portaudio-2.0)

add_executable(term_view
	main.cpp
	database.cpp
	track.cpp
	viewer.cpp
	player.cpp
	pa_ringbuffer.c
)

target_link_libraries(term_view PRIVATE
	SQLite3::SQLite3
	PkgConfig::TAGLIB
	PkgConfig::AVCODEC
	PkgConfig::AVFORMAT
	PkgConfig::AVUTIL
	PkgConfig::SWRESAMPLE
	PkgConfig::PORTAUDIO
)
