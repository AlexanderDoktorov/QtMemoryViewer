cmake_minimum_required(VERSION 3.16)
project(MemoryViewApp)

# Find Qt 6 installation
set(CMAKE_PREFIX_PATH "A:\Qt6.7.0\Qt\6.7.0\msvc2019_64\lib\cmake")

# Find Qt packages
find_package(Qt6 REQUIRED COMPONENTS Widgets)

# Add the executable
add_executable(MemoryViewApp main.cpp)

# Link Qt libraries
target_link_libraries(MemoryViewApp PRIVATE Qt6::Widgets)