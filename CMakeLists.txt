cmake_minimum_required(VERSION 3.14)
project(TerminalView LANGUAGES CXX)

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

add_executable(term_view
	main.cpp
	database.cpp
	track.cpp
	viewer.cpp
)

target_link_libraries(term_view PRIVATE
	SQLite3::SQLite3
	PkgConfig::TAGLIB
)
