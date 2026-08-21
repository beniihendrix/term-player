# term-player

term-player is a C++ terminal application that scans a directory for `.mp3` and `.m4a` files, extracts their metadata, and indexes them into a local SQLite database for easy terminal-based navigation and playback.

## Prerequisites

To build and run this project, you will need the following installed on your system:

*   **C++17 Compiler** (e.g., GCC, Clang)
*   **CMake** (Minimum version 3.14)
*   **SQLite3**
*   **TagLib**
*   **PortAudio**
*   **ffmpeg**

### For Mac Users

The way I downloaded all my libraries for through `homebrew` by using the command
```
brew install cmake taglib ffmpeg portaudio sqlite3
```

## Build Instructions

This project uses CMake for its build system.

1. Clone the repository:
   ```bash
   git clone https://github.com/beniihendrix/term-player.git
   cd term-player

2. Create a build directory using cmake:
    ```bash

    cmake -B build

3. Generate the build files and compile the project:
    ```bash

    cmake --build

## Usage

After building the project, you can run the executable from the __build__ directory:
```bash

./Player_UI