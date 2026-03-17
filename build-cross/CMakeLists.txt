cmake_minimum_required(VERSION 3.16)
project(LG)

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
install(TARGETS LG RUNTIME DESTINATION bin)

option(USE_STB "Enable stb_image support" ON)

include(FetchContent)
include(ExternalProject)

set(STRIP_ARGS "")
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(STRIP_ARGS --strip)
endif()

# GLEW
ExternalProject_Add(glew_ep
    URL https://github.com/nigels-com/glew/releases/download/glew-2.3.1/glew-2.3.1.zip
    CMAKE_ARGS
        -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
        -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX} -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DBUILD_UTILS=OFF
    SOURCE_SUBDIR build/cmake
    INSTALL_COMMAND ${CMAKE_COMMAND} --install <BINARY_DIR> ${STRIP_ARGS}
)

# GLUT
ExternalProject_Add(freeglut_ep
    URL https://github.com/freeglut/freeglut/releases/download/v3.8.0/freeglut-3.8.0.tar.gz
    CMAKE_ARGS
        -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
        -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX} -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DFREEGLUT_REPLACE_GLUT=ON -DFREEGLUT_BUILD_DEMOS=OFF
    INSTALL_COMMAND ${CMAKE_COMMAND} --install <BINARY_DIR> ${STRIP_ARGS}
)

# GLM
FetchContent_Declare(glm URL https://github.com/g-truc/glm/releases/download/1.0.3/glm-1.0.3.zip)
FetchContent_MakeAvailable(glm)
target_include_directories(LG SYSTEM PUBLIC ${glm_SOURCE_DIR})

# STB
if(USE_STB)
    FetchContent_Declare(stb GIT_REPOSITORY https://github.com/nothings/stb.git GIT_TAG master)
    FetchContent_MakeAvailable(stb)
    target_compile_definitions(LG PRIVATE HAS_STB NEED_STB_IMPLEMENTATION)
    target_include_directories(LG SYSTEM PRIVATE ${stb_SOURCE_DIR})
endif()

# Link libraries for OpenGL, GLEW, GLUT, GLM, and stb_image
add_dependencies(LG glew_ep freeglut_ep)
target_include_directories(LG SYSTEM PUBLIC ${CMAKE_INSTALL_PREFIX}/include)
target_link_directories(LG PUBLIC ${CMAKE_INSTALL_PREFIX}/lib)
target_link_libraries(LG PRIVATE glut)
target_link_libraries(LG PUBLIC glew32 opengl32)

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
endforeach()

if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    install(FILES ${CROSS_RUNTIME_LIBS} DESTINATION bin PERMISSIONS
        OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
    install(CODE "
        foreach(f ${CROSS_RUNTIME_LIBS})
            get_filename_component(name \"\${f}\" NAME)
            execute_process(COMMAND ${CMAKE_STRIP} \"\${CMAKE_INSTALL_PREFIX}/bin/\${name}\")
        endforeach()
    ")
endif()
