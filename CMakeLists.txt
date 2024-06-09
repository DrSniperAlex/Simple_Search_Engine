cmake_minimum_required(VERSION 3.12)

project(
        search_engine
        VERSION 1.0
        LANGUAGES CXX
)
set(CMAKE_CXX_STANDARD 20)

add_subdirectory(lib)
add_subdirectory(bin)

include(FetchContent)
set(FETCHCONTENT_UPDATES_DISCONNECTED TRUE)
FetchContent_Declare(
        ftxui
        GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
        GIT_TAG main # Important: Specify a version or a commit hash here.
)
FetchContent_MakeAvailable(ftxui)

target_link_directories(search_engine PRIVATE tlib)

enable_testing()
add_subdirectory(tests)