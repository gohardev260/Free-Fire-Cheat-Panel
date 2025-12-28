cmake_minimum_required(VERSION 3.10)
project(FreeFireCheatPanel)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(src)
include_directories(src/gui)
include_directories(src/imgui)
include_directories(src/memory)
include_directories(src/security)

# Gather source files
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Create executable
# WIN32 option prevents a console window from popping up (for a GUI app)
# If you want to see console output for debugging, remove "WIN32"
add_executable(${PROJECT_NAME} WIN32 ${SOURCES})

# Link libraries
if(MSVC)
    target_link_libraries(${PROJECT_NAME} d3d11.lib d3dcompiler.lib dwmapi.lib dxgi.lib user32.lib kernel32.lib advapi32.lib)
elseif(MINGW)
    target_link_libraries(${PROJECT_NAME} d3d11 d3dcompiler_47 dwmapi)
endif()
