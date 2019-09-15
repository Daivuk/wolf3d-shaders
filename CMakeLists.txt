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
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}" "/MP")     # Multi core in VS
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4101")   # Unused local variable
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4068")   # Unknown pragma
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4018")   # Sign compare mismatch, there's too many...
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4102")   # Unreferenced label
endif()

# Debug/Release flags
if (CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-D_DEBUG)
endif()

#justwindowsthings
if (MSVC)
    add_definitions(-DNOMINMAX)                         # Don't want Win32's min and max defined
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)          # i.e: For sprintf instead of s_sprintf
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmax-errors=5 -ferror-limit=2")
endif()

# Project files
file(GLOB src_wolf3d_shaders ./src/wolf3d_shaders/*.*)
file(GLOB src_wolfenstein_3d ./src/wolfenstein_3d/*.CPP ./src/wolfenstein_3d/*.H)

list(APPEND includes PUBLIC 
    ./src/wolf3d_shaders/
    ./src/wolfenstein_3d/
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
list(APPEND src_thirdparty ./thirdparty/gl3w/src/gl3w.c)

# tinyfiledialogs
list(APPEND includes PUBLIC ./thirdparty/tinyfiledialogs/)
list(APPEND src_thirdparty ./thirdparty/tinyfiledialogs/tinyfiledialogs.c)

# dirent
if (WIN32)
    list(APPEND includes PUBLIC ./thirdparty/dirent/)
endif()

# Dear imgui
list(APPEND includes PUBLIC ./thirdparty/imgui/ ./thirdparty/imgui/examples/)
list(APPEND src_thirdparty
    ./thirdparty/imgui/imgui_demo.cpp
    ./thirdparty/imgui/imgui_draw.cpp
    ./thirdparty/imgui/imgui_internal.h
    ./thirdparty/imgui/imgui.cpp
    ./thirdparty/imgui/imgui.h
    ./thirdparty/imgui/stb_rect_pack.h
    ./thirdparty/imgui/stb_textedit.h
    ./thirdparty/imgui/stb_truetype.h
)

# jsoncpp
list(APPEND includes PUBLIC ./thirdparty/jsoncpp/include/)
list(APPEND src_thirdparty
    ./thirdparty/jsoncpp/src/lib_json/json_reader.cpp
    ./thirdparty/jsoncpp/src/lib_json/json_tool.h
    ./thirdparty/jsoncpp/src/lib_json/json_value.cpp
    ./thirdparty/jsoncpp/src/lib_json/json_valueiterator.inl
    ./thirdparty/jsoncpp/src/lib_json/json_writer.cpp
)

#------------------------------------------------------------------------------
# Exe
#------------------------------------------------------------------------------

# Wolf3DShaders.exe, use WinMain on Windows
source_group("wolf3d_shaders" FILES ${src_wolf3d_shaders})
source_group("wolfenstein_3d" FILES ${src_wolfenstein_3d})
source_group("thirdparty" FILES ${src_thirdparty})
add_executable(${PROJECT_NAME} WIN32 
    ${src_wolf3d_shaders}
    ${src_wolfenstein_3d}
    ${src_thirdparty})

# Work dir, copy your WAD files there
set_property(TARGET ${PROJECT_NAME} PROPERTY
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/")

# Lib/Headers
target_include_directories(${PROJECT_NAME} ${includes})
target_link_libraries(${PROJECT_NAME} ${libs})
