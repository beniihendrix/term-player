# term-player

term-player is a C++ command-line application that scans a directory for `.mp3` files, extracts their metadata, and indexes them into a local SQLite database for easy terminal-based navigation and playback management.

## Prerequisites

To build and run this project, you will need the following installed on your system:

*   **C++17 Compiler** (e.g., GCC, Clang)
*   **CMake** (Minimum version 3.14)
*   **SQLite3**
*   **TagLib** (e.g., via Homebrew: `brew install taglib`)

## Build Instructions

This project uses CMake for its build system. It is recommended to use an out-of-source build:

1. Clone the repository:
   ```bash
   git clone <your-repository-url>
   cd term-view

2. Create a build directory and navigate into it:
    ```bash

    mkdir build
    cd build

3. Generate the build files and compile the project:
    ```bash

    cmake ..
    make

## Usage

After building the project, you can run the executable from the build directory:
```bash

./term_view
