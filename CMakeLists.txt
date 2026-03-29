cmake_minimum_required(VERSION 3.16)
project(LG)

# Use C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# -----------------------------
# libLG
# -----------------------------
file(GLOB LIB_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
if(MSVC)
    add_library(LG STATIC ${LIB_SOURCES})
else()
    add_library(LG SHARED ${LIB_SOURCES})
    target_link_options(LG PUBLIC -Wl,--rpath=$ORIGIN/../lib)
endif()
target_include_directories(LG PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
install(TARGETS LG PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

# Enable warnings
if(MSVC)
    target_compile_options(LG PUBLIC /W4)
else()
    target_compile_options(LG PUBLIC -Wall -Wshadow -Wextra -Wpedantic -Wconversion -Wno-sign-conversion)
endif()

option(USE_MAGICK "Enable GraphicsMagick++ support" ON)
option(USE_STB "Enable stb_image support" ON)
option(USE_ICU "Enable ICU support" ON)

# Link libraries for OpenGL, GLEW, GLUT, GLM, GraphicsMagick++, stb_image, and ICU
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLUT REQUIRED)
find_package(glm REQUIRED)
find_package(PkgConfig QUIET)
target_link_libraries(LG PRIVATE GLUT::GLUT)
target_link_libraries(LG PUBLIC glm::glm GLEW::GLEW OpenGL::GL)
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
if(USE_ICU)
    find_package(ICU QUIET COMPONENTS uc i18n)
    if(ICU_FOUND)
        message(STATUS "Found ICU: ${ICU_VERSION}")
        target_compile_definitions(LG PRIVATE HAS_ICU)
        target_link_libraries(LG PRIVATE ICU::uc ICU::i18n)
    else()
        message(STATUS "ICU not found")
    endif()
endif()

# -----------------------------
# Examples
# -----------------------------
file(GLOB EXAMPLE_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/examples/*.cpp")
foreach(EXAMPLE_SOURCE ${EXAMPLE_SOURCES})
    get_filename_component(EXAMPLE_NAME ${EXAMPLE_SOURCE} NAME_WE)
    add_executable(${EXAMPLE_NAME} ${EXAMPLE_SOURCE})
    install(TARGETS ${EXAMPLE_NAME})
    target_link_libraries(${EXAMPLE_NAME} PRIVATE LG)
endforeach()

# -----------------------------
# Resources
# -----------------------------
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/share" DESTINATION .)
