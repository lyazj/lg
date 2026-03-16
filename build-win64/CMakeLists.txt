cmake_minimum_required(VERSION 3.16)

set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

set(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)
set(CMAKE_RC_COMPILER x86_64-w64-mingw32-windres)

set(CMAKE_FIND_ROOT_PATH /usr/x86_64-w64-mingw32)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

project(LG)

file(ARCHIVE_EXTRACT INPUT "../install-win64.tar.gz" DESTINATION "..")
set(CMAKE_INSTALL_PREFIX "../install-win64")

# Use C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable warnings
add_compile_options(-Wall -Wshadow -Wextra -Wpedantic -Wconversion -Wno-sign-conversion)

# -----------------------------
# Collect all source files
# -----------------------------
file(GLOB ALL_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/../*.cpp")

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
add_library(LG SHARED ${LIB_SOURCES})

option(USE_STB "Enable stb_image support" ON)
option(CI_BUILD "Enable CI-specific build settings" OFF)

# Link libraries for OpenGL, GLEW, GLUT, GLM, and stb_image
target_include_directories(LG SYSTEM PUBLIC ../install-win64/include)
target_link_directories(LG PUBLIC ../install-win64/lib)
target_link_libraries(LG PUBLIC freeglut glew32 opengl32)
file(COPY /usr/include/glm DESTINATION ../install-win64/include)
if(USE_STB)
    file(COPY /usr/include/stb DESTINATION ../install-win64/include)
    target_compile_definitions(LG PRIVATE HAS_STB NEED_STB_IMPLEMENTATION)
    target_include_directories(LG SYSTEM PRIVATE ../install-win64/include/stb)
endif()

# -----------------------------
# Create executable for each example
# -----------------------------
foreach(EXAMPLE_SRC ${EXAMPLE_SOURCES})
    get_filename_component(EXAMPLE_NAME ${EXAMPLE_SRC} NAME_WE)
    add_executable(${EXAMPLE_NAME} ${EXAMPLE_SRC})
    target_link_libraries(${EXAMPLE_NAME} PRIVATE LG)
endforeach()

install(TARGETS LG RUNTIME DESTINATION bin)
foreach(EXAMPLE_SRC ${EXAMPLE_SOURCES})
    get_filename_component(EXAMPLE_NAME ${EXAMPLE_SRC} NAME_WE)
    install(TARGETS ${EXAMPLE_NAME} RUNTIME DESTINATION bin)
endforeach()
install(FILES "../world.jpg" DESTINATION bin)
install(FILES
    "/usr/lib/gcc/x86_64-w64-mingw32/13-win32/libgcc_s_seh-1.dll"
    "/usr/lib/gcc/x86_64-w64-mingw32/13-win32/libstdc++-6.dll"
    "/usr/x86_64-w64-mingw32/lib/libwinpthread-1.dll"
    DESTINATION bin PERMISSIONS
    OWNER_READ OWNER_WRITE OWNER_EXECUTE
    GROUP_READ             GROUP_EXECUTE
    WORLD_READ             WORLD_EXECUTE)
