# CMake build script for building an executable application.
#
# Building from source:
# > cmake . -Bbuild -DLIBDIR:STRING=<library directory>
# > cmake --build .

cmake_minimum_required(VERSION 3.5)
project(Lista_02
        VERSION 0.0.0)

enable_language(CXX)
set(CMAKE_CXX_STANDARD 11)

# Add header directory to project
include_directories(include)

# Add a submodule to the project
add_subdirectory(lib/vhdf-1.0.0/)

# Add an external library to the project
# find_package(LIB REQUIRED)
# include_directories(${LIB_INCLUDE_DIRS})

# Declare source files for build
set(
    SOURCE_FILES
    src/main.cpp
    src/Data.cpp
    src/DataBlock.cpp
    src/Parser.cpp
    src/Query.cpp
    src/Registro.cpp
    src/Util.cpp
    src/MemoryWrapper.cpp
    src/Campo.cpp
    src/Hash/Insert.cpp
    src/Hash/Select.cpp
    src/Hash/Delete.cpp
    src/Hash/Test.cpp
    src/Heap/Insert.cpp
    src/Heap/Select.cpp
    src/Heap/Delete.cpp
    src/Heap/Test.cpp
    src/Ordered/Insert.cpp
    src/Ordered/Select.cpp
    src/Ordered/Delete.cpp
    src/Ordered/Test.cpp
        src/RegistroPartido.cpp src/Equijoin.cpp include/Equijoin.hpp)

# Set the name of the built executable
set(EXE_NAME "${PROJECT_NAME}-${PROJECT_VERSION}")

add_executable(${EXE_NAME} ${SOURCE_FILES})
set_target_properties(
    ${EXE_NAME} PROPERTIES
    VERSION 0
)

target_include_directories(${EXE_NAME} PUBLIC ${INCLUDE_DIRECTORIES})

# Link with an external package
# target_link_libraries(${EXE_NAME} ${LIB_LIBRARIES})

# Link with a submodule
target_link_libraries(${EXE_NAME} vhdf)

# Copy dynamic libraries to executable directory if necessary
#add_custom_command(
#    TARGET ${EXE_NAME} POST_BUILD
#    COMMAND ${CMAKE_COMMAND} -E copy_if_different
#        $<TARGET_FILE_DIR:vhdf>/vhdf.dll
#        $<TARGET_FILE_DIR:${EXE_NAME}>
#)
