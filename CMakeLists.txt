cmake_minimum_required(VERSION 3.24)
project(LG)

option(FIND_GLM "Search for native GLM" ON)
option(FIND_STB "Search for native STB" ON)
option(USE_MAGICK "Enable GraphicsMagick++" ON)
option(USE_STB "Enable STB" ON)
option(USE_ICU "Enable ICU" ON)

# Use C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# -----------------------------
# libLG
# -----------------------------
file(GLOB LIB_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
add_library(LG SHARED ${LIB_SOURCES})
if(APPLE)
    target_link_options(LG PUBLIC "-Wl,-rpath,@loader_path/../lib")
elseif(UNIX)
    target_link_options(LG PUBLIC "-Wl,-rpath=\$ORIGIN/../lib")
endif()
target_include_directories(LG PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
install(TARGETS LG)

# Enable warnings
target_compile_options(LG PUBLIC -Wall -Wshadow -Wextra -Wpedantic -Wconversion -Wno-sign-conversion)

# Strip symbols in release builds
set(STRIP_ARGS "")
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(STRIP_ARGS --strip)
endif()

include(FetchContent)
include(ExternalProject)
find_package(PkgConfig QUIET)

# GLEW
ExternalProject_Add(glew_ep
    URL https://github.com/nigels-com/glew/releases/download/glew-2.3.1/glew-2.3.1.zip DOWNLOAD_EXTRACT_TIMESTAMP
    CMAKE_ARGS
        -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
        -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX} -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DBUILD_UTILS=OFF
    SOURCE_SUBDIR build/cmake
    INSTALL_COMMAND ${CMAKE_COMMAND} --install <BINARY_DIR> ${STRIP_ARGS}
)

# GLUT (build v3.8.0 from source; ensures unicode support)
ExternalProject_Add(freeglut_ep
    URL https://github.com/freeglut/freeglut/releases/download/v3.8.0/freeglut-3.8.0.tar.gz DOWNLOAD_EXTRACT_TIMESTAMP
    CMAKE_ARGS
        -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
        -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX} -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DFREEGLUT_REPLACE_GLUT=ON -DFREEGLUT_BUILD_DEMOS=OFF
    INSTALL_COMMAND ${CMAKE_COMMAND} --install <BINARY_DIR> ${STRIP_ARGS}
)

# GLM
if(FIND_GLM AND PkgConfig_FOUND)
    pkg_check_modules(glm QUIET glm)
endif()
if(glm_FOUND)
    message(STATUS "Found GLM: ${glm_INCLUDE_DIRS}")
    target_include_directories(LG SYSTEM PUBLIC ${glm_INCLUDE_DIRS})
    target_link_libraries(LG PUBLIC ${glm_LIBRARIES})
else()
    message(STATUS "GLM not found, fetching from GitHub...")
    FetchContent_Declare(glm URL https://github.com/g-truc/glm/releases/download/1.0.3/glm-1.0.3.zip)
    FetchContent_MakeAvailable(glm)
    message(STATUS "Fetched GLM: ${glm_SOURCE_DIR}")
    target_include_directories(LG SYSTEM PUBLIC ${glm_SOURCE_DIR})
endif()

# GraphicsMagick++
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

# STB
if(USE_STB)
    if(FIND_STB AND PkgConfig_FOUND)
        pkg_check_modules(stb QUIET stb)
    endif()
    if(stb_FOUND)
        message(STATUS "Found STB: ${stb_INCLUDE_DIRS}")
        target_compile_definitions(LG PRIVATE HAS_STB)
        target_include_directories(LG SYSTEM PRIVATE ${stb_INCLUDE_DIRS})
        target_link_libraries(LG PRIVATE ${stb_LIBRARIES})
    else()
        message(STATUS "STB not found, fetching from GitHub...")
        include(FetchContent)
        FetchContent_Declare(stb GIT_REPOSITORY https://github.com/nothings/stb.git GIT_TAG master)
        FetchContent_MakeAvailable(stb)
        message(STATUS "Fetched STB: ${stb_SOURCE_DIR}")
        target_compile_definitions(LG PRIVATE HAS_STB NEED_STB_IMPLEMENTATION)
        target_include_directories(LG SYSTEM PRIVATE ${stb_SOURCE_DIR})
    endif()
endif()

# ICU
if(USE_ICU)
    find_package(ICU QUIET COMPONENTS uc i18n)
    if(ICU_FOUND)
        message(STATUS "Found ICU: ${ICU_INCLUDE_DIRS}")
        target_compile_definitions(LG PRIVATE HAS_ICU)
        target_include_directories(LG SYSTEM PRIVATE ${ICU_INCLUDE_DIRS})
        target_link_libraries(LG PRIVATE ICU::uc ICU::i18n)
    else()
        message(STATUS "ICU not found")
    endif()
endif()

# Link libraries
add_dependencies(LG glew_ep freeglut_ep)
target_include_directories(LG SYSTEM PUBLIC ${CMAKE_INSTALL_PREFIX}/include)
target_link_directories(LG PUBLIC ${CMAKE_INSTALL_PREFIX}/lib)
target_link_libraries(LG PRIVATE glut)
if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    target_link_libraries(LG PUBLIC glew32 opengl32)
else()
    find_package(OpenGL REQUIRED)
    target_link_libraries(LG PUBLIC GLEW OpenGL::GL)
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

# -----------------------------
# Resources
# -----------------------------
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/share" DESTINATION .)
