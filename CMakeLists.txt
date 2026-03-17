cmake_minimum_required(VERSION 3.16)
project(LG)

# Use C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable warnings
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wshadow -Wextra -Wpedantic -Wconversion -Wno-sign-conversion)
endif()

# -----------------------------
# Collect all source files
# -----------------------------
file(GLOB ALL_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

set(LIB_SOURCES "")
set(EXAMPLE_SOURCES "")

foreach(SRC ${ALL_SOURCES})
    get_filename_component(FILENAME ${SRC} NAME_WE)
    if(FILENAME MATCHES ".*Example$")
        list(APPEND EXAMPLE_SOURCES ${SRC})
    else()
        list(APPEND LIB_SOURCES ${SRC})
    endif()
endforeach()

# -----------------------------
# Create library from non-example sources
# -----------------------------
if(MSVC)
    add_library(LG STATIC ${LIB_SOURCES})
else()
    add_library(LG SHARED ${LIB_SOURCES})
endif()
install(TARGETS LG RUNTIME DESTINATION bin)

option(USE_MAGICK "Enable GraphicsMagick++ support" ON)
option(USE_STB "Enable stb_image support" ON)

# Link libraries for OpenGL, GLEW, GLUT, GLM, GraphicsMagick++, and stb_image
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLUT REQUIRED)
find_package(glm REQUIRED)
find_package(PkgConfig QUIET)
target_link_libraries(LG PUBLIC glm::glm GLUT::GLUT GLEW::GLEW OpenGL::GL)
if(USE_MAGICK)
    if(PkgConfig_FOUND)
        pkg_check_modules(GM QUIET GraphicsMagick++)
    endif()
    if(GM_FOUND)
        message(STATUS "Found GraphicsMagick++: ${GM_INCLUDE_DIRS}")
        target_compile_definitions(LG PRIVATE HAS_MAGICK)
        target_include_directories(LG SYSTEM PRIVATE ${GM_INCLUDE_DIRS})
        target_link_libraries(LG PRIVATE ${GM_LIBRARIES})
    else()
        message(STATUS "GraphicsMagick++ not found")
    endif()
endif()
if(USE_STB)
    if(PkgConfig_FOUND)
        pkg_check_modules(stb QUIET stb)
    endif()
    if(stb_FOUND)
        message(STATUS "Found stb: ${stb_INCLUDE_DIRS}")
        target_compile_definitions(LG PRIVATE HAS_STB)
        target_include_directories(LG SYSTEM PRIVATE ${stb_INCLUDE_DIRS})
        target_link_libraries(LG PRIVATE ${stb_LIBRARIES})
    else()
        message(STATUS "stb not found, fetching from GitHub...")
        include(FetchContent)
        FetchContent_Declare(stb GIT_REPOSITORY https://github.com/nothings/stb.git GIT_TAG master)
        FetchContent_MakeAvailable(stb)
        message(STATUS "Fetched stb: ${stb_SOURCE_DIR}")
        target_compile_definitions(LG PRIVATE HAS_STB NEED_STB_IMPLEMENTATION)
        target_include_directories(LG SYSTEM PRIVATE ${stb_SOURCE_DIR})
    endif()
endif()

function(fetch_resource URL OUT_FILE)
    if(NOT OUT_FILE)
        get_filename_component(OUT_FILE ${URL} NAME)
    endif()
    set(OUT_PATH "${CMAKE_BINARY_DIR}/${OUT_FILE}")
    if(NOT EXISTS "${OUT_PATH}")
        message(STATUS "Downloading ${OUT_FILE} from ${URL}...")
        file(DOWNLOAD "${URL}" "${OUT_PATH}" SHOW_PROGRESS)
    else()
        message(STATUS "${OUT_FILE} already exists, skipping download.")
    endif()
    install(FILES ${OUT_PATH} DESTINATION bin)
endfunction()

fetch_resource("https://upload.wikimedia.org/wikipedia/commons/3/3e/Equirectangular-projection-topographic-world.jpg" "world.jpg")

# -----------------------------
# Create executable for each example
# -----------------------------
foreach(EXAMPLE_SRC ${EXAMPLE_SOURCES})
    get_filename_component(EXAMPLE_NAME ${EXAMPLE_SRC} NAME_WE)
    add_executable(${EXAMPLE_NAME} ${EXAMPLE_SRC})
    install(TARGETS ${EXAMPLE_NAME} RUNTIME DESTINATION bin)
    target_link_libraries(${EXAMPLE_NAME} PRIVATE LG)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        target_link_options(${EXAMPLE_NAME} PRIVATE -Wl,--rpath=../lib)
    endif()
endforeach()
