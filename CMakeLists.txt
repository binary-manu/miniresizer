cmake_minimum_required(VERSION 3.3)

# The project is written in C++, but the C compiler is nneded for some tests
# against FFMPEG.
project(
    miniresizer
    DESCRIPTION "Calculate video crop/resize parameters"
    VERSION 0.1.0
    LANGUAGES C CXX
)

add_subdirectory(src)
