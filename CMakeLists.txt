cmake_minimum_required(VERSION 3.0.0)

# Define LINUX
if (UNIX AND NOT APPLE)
    set(LINUX 1)
endif()

# Setup modules path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")

# Project name
project(Wolf3DShaders)

# Some compiler flags
set(CMAKE_CXX_STANDARD 14) # C++14
if (MSVC)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}" "/MP") # Multi core in VS (This doesn't work anymore...)
endif()

# Debug/Release flags
if (CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-D_DEBUG)
endif()
# SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}" "RELEASE" "NDEBUG")

#justwindowsthings
if (MSVC)
    add_definitions(-DNOMINMAX)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4996")
    # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4101")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmax-errors=5 -ferror-limit=2")
endif()

# Project files
file(GLOB sources
    ./src/*.cpp
)

list(APPEND includes PUBLIC 
    ./src/
)

#------------------------------------------------------------------------------
# Third parties
#------------------------------------------------------------------------------

# OpenGL
find_package(OpenGL REQUIRED)
list(APPEND includes PUBLIC ${OPENGL_INCLUDE_DIR})
list(APPEND libs ${OPENGL_LIBRARIES})

# SDL2
set(HAVE_LIBC ON)
add_subdirectory(./thirdparty/SDL-mirror/)
list(APPEND libs SDL2-static)
list(APPEND includes PUBLIC ./thirdparty/SDL-mirror/include/)

# gl3w
list(APPEND includes PUBLIC ./thirdparty/gl3w/include/)
list(APPEND srcthirdparty ./thirdparty/gl3w/src/gl3w.c)

# tinyfiledialogs
list(APPEND includes PUBLIC ./thirdparty/tinyfiledialogs/)
list(APPEND srcthirdparty ./thirdparty/tinyfiledialogs/tinyfiledialogs.c)

#------------------------------------------------------------------------------
# Exe
#------------------------------------------------------------------------------

# Wolf3DShaders.exe, use WinMain on Windows
add_executable(Wolf3DShaders WIN32 
    ${sources}
    ${srcthirdparty})

# Work dir, copy your WAD files there
set_property(TARGET Wolf3DShaders PROPERTY
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/")

# Lib/Headers
target_include_directories(Wolf3DShaders ${includes})
target_link_libraries(Wolf3DShaders ${libs})
