#!/bin/sh

set -e

BUILD_DIR="build"
CMAKE_BUILD_TYPE="Release"

show_help() {
    printf "Usage: %s [options]\n" "$0"
    printf "Options:\n"
    printf "  -c, --clean     Clean the build directory before building\n"
    printf "  -v, --verbose   Show additional output\n"
    printf "  -q, --quiet     Suppress all output\n"
    printf "  -h, --help      Show this help message and exit\n"
    printf "Example:\n"
    printf "  %s --clean --verbose\n" "$0"
}

clean_build() {
    if [ -d "$BUILD_DIR" ]; then
        printf "Cleaning the build directory...\n"
        rm -rf "$BUILD_DIR"
    else
        printf "Build directory not found. No need to clean.\n"
    fi
}

initialize_environment() {
    if ! command -v cmake > /dev/null 2>&1; then
        printf "CMake not found. Please ensure that CMake is installed and available in your PATH.\n"
        exit 1
    fi
    if ! command -v make > /dev/null 2>&1; then
        printf "Make not found. Please ensure that Make is installed and available in your PATH.\n"
        exit 1
    fi

    if [ ! -d "$BUILD_DIR" ]; then
        printf "Creating build directory...\n"
        mkdir -p "$BUILD_DIR"
    fi

    cd "$BUILD_DIR" || exit
}

invoke_build() {
    printf "Configuring the build...\n"
    cmake .. -DCMAKE_BUILD_TYPE="$CMAKE_BUILD_TYPE"
    printf "Building the project...\n"
    make
    printf "Build successful! Binary is located in bin/\n"
}

CLEAN=false
VERBOSE=false
QUIET=false

while [ $# -gt 0 ]; do
    case $1 in
        -c|--clean)
            CLEAN=true
            shift
            ;;
        -v|--verbose)
            VERBOSE=true
            shift
            ;;
        -q|--quiet)
            QUIET=true
            shift
            ;;
        -h|--help)
            show_help
            exit 0
            ;;
        *)
            printf "Unknown option: %s\n" "$1"
            show_help
            exit 1
            ;;
    esac
done

if $QUIET; then
    exec > /dev/null 2>&1
fi

initialize_environment

if $CLEAN; then
    clean_build
fi

invoke_build

if $VERBOSE; then
    printf "Script execution complete.\n"
fi
