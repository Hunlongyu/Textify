cmake_minimum_required(VERSION 4.0)

project(Textify LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (MSVC)
    add_compile_options("/utf-8")
    set(CMAKE_MSVC_SCAN_DEPENDENCY OFF)
endif()

include(FetchContent)
FetchContent_Declare(
    sw
    GIT_REPOSITORY https://github.com/Mzying2001/sw
    GIT_TAG        0.0.7
    SOURCE_SUBDIR  sw
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(sw)

include(FetchContent)
FetchContent_Declare(
    config 
    GIT_REPOSITORY  https://github.com/Hunlongyu/config.git 
    GIT_TAG         main
    GIT_SHALLOW     TRUE
)
set(BUILD_CONFIG_EXAMPLES OFF CACHE BOOL "")
FetchContent_MakeAvailable(config)

set(Files 
    "src/main.cpp"
    "src/views/home/home.cpp"
    "src/mouse_hook/mouse_hook.cpp"
    "src/utils/utils.cpp"
)

enable_language(RC)
set(CMAKE_RC_COMPILER_INIT windres)

if(CMAKE_BUILD_TYPE STREQUAL "Debug") 
    add_executable(${PROJECT_NAME} ${Files})
else()
    add_executable(${PROJECT_NAME} WIN32 ${Files})
endif()

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/assets
)

target_link_libraries(${PROJECT_NAME} PRIVATE 
    sw
    config
    ole32
    oleacc
)
