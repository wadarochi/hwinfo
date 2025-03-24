cmake_minimum_required(VERSION 3.5)

project(system_info VERSION 1.0.0 LANGUAGES CXX)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries" FORCE)

include(FetchContent)

find_package(fmt QUIET)

if (NOT fmt_FOUND)
    message(" -> libfmt not found. Fetching from github...")
    include(FetchContent)
    FetchContent_Declare(
        fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG 11.0.1)
    FetchContent_MakeAvailable(fmt)
else ()
    message(" -> using installed libfmt")
endif ()

find_package(cxxopts QUIET)

if (NOT cxxopts_FOUND)
    message(" -> cxxopts not found. Fetching from github...")
    FetchContent_Declare(
        cxxopts
        GIT_REPOSITORY https://github.com/jarro2783/cxxopts.git
        GIT_TAG v3.2.1)
    FetchContent_MakeAvailable(cxxopts)
else ()
    message(" -> using installed cxxopts")
endif ()


find_package(nlohmann_json QUIET)

if (NOT nlohmann_json_FOUND)
    message(" -> nlohmann_json not found. Fetching from github...")
    FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.11.3)
    FetchContent_MakeAvailable(nlohmann_json)
else ()
    message(" -> using installed nlohmann_json")
endif ()


add_executable(system_info system_infoMain.cpp)

target_link_libraries(system_info PRIVATE lfreist-hwinfo::hwinfo fmt::fmt cxxopts::cxxopts nlohmann_json::nlohmann_json)

install(TARGETS system_info)
