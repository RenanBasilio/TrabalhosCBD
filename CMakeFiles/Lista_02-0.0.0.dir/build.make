# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.13

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /opt/cmake-3.13.0-rc1-Linux-x86_64/bin/cmake

# The command to remove a file.
RM = /opt/cmake-3.13.0-rc1-Linux-x86_64/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/rebello/TrabalhosCBD

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/rebello/TrabalhosCBD

# Include any dependencies generated for this target.
include CMakeFiles/Lista_02-0.0.0.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/Lista_02-0.0.0.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/Lista_02-0.0.0.dir/flags.make

CMakeFiles/Lista_02-0.0.0.dir/src/main.cpp.o: CMakeFiles/Lista_02-0.0.0.dir/flags.make
CMakeFiles/Lista_02-0.0.0.dir/src/main.cpp.o: src/main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/rebello/TrabalhosCBD/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/Lista_02-0.0.0.dir/src/main.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Lista_02-0.0.0.dir/src/main.cpp.o -c /home/rebello/TrabalhosCBD/src/main.cpp

CMakeFiles/Lista_02-0.0.0.dir/src/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Lista_02-0.0.0.dir/src/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/rebello/TrabalhosCBD/src/main.cpp > CMakeFiles/Lista_02-0.0.0.dir/src/main.cpp.i

CMakeFiles/Lista_02-0.0.0.dir/src/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Lista_02-0.0.0.dir/src/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/rebello/TrabalhosCBD/src/main.cpp -o CMakeFiles/Lista_02-0.0.0.dir/src/main.cpp.s

CMakeFiles/Lista_02-0.0.0.dir/src/Data.cpp.o: CMakeFiles/Lista_02-0.0.0.dir/flags.make
CMakeFiles/Lista_02-0.0.0.dir/src/Data.cpp.o: src/Data.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/rebello/TrabalhosCBD/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/Lista_02-0.0.0.dir/src/Data.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Lista_02-0.0.0.dir/src/Data.cpp.o -c /home/rebello/TrabalhosCBD/src/Data.cpp

CMakeFiles/Lista_02-0.0.0.dir/src/Data.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Lista_02-0.0.0.dir/src/Data.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/rebello/TrabalhosCBD/src/Data.cpp > CMakeFiles/Lista_02-0.0.0.dir/src/Data.cpp.i

CMakeFiles/Lista_02-0.0.0.dir/src/Data.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Lista_02-0.0.0.dir/src/Data.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/rebello/TrabalhosCBD/src/Data.cpp -o CMakeFiles/Lista_02-0.0.0.dir/src/Data.cpp.s

CMakeFiles/Lista_02-0.0.0.dir/src/BlockIO.cpp.o: CMakeFiles/Lista_02-0.0.0.dir/flags.make
CMakeFiles/Lista_02-0.0.0.dir/src/BlockIO.cpp.o: src/BlockIO.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/rebello/TrabalhosCBD/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/Lista_02-0.0.0.dir/src/BlockIO.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Lista_02-0.0.0.dir/src/BlockIO.cpp.o -c /home/rebello/TrabalhosCBD/src/BlockIO.cpp

CMakeFiles/Lista_02-0.0.0.dir/src/BlockIO.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Lista_02-0.0.0.dir/src/BlockIO.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/rebello/TrabalhosCBD/src/BlockIO.cpp > CMakeFiles/Lista_02-0.0.0.dir/src/BlockIO.cpp.i

CMakeFiles/Lista_02-0.0.0.dir/src/BlockIO.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Lista_02-0.0.0.dir/src/BlockIO.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/rebello/TrabalhosCBD/src/BlockIO.cpp -o CMakeFiles/Lista_02-0.0.0.dir/src/BlockIO.cpp.s

CMakeFiles/Lista_02-0.0.0.dir/src/Parser.cpp.o: CMakeFiles/Lista_02-0.0.0.dir/flags.make
CMakeFiles/Lista_02-0.0.0.dir/src/Parser.cpp.o: src/Parser.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/rebello/TrabalhosCBD/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/Lista_02-0.0.0.dir/src/Parser.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Lista_02-0.0.0.dir/src/Parser.cpp.o -c /home/rebello/TrabalhosCBD/src/Parser.cpp

CMakeFiles/Lista_02-0.0.0.dir/src/Parser.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Lista_02-0.0.0.dir/src/Parser.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/rebello/TrabalhosCBD/src/Parser.cpp > CMakeFiles/Lista_02-0.0.0.dir/src/Parser.cpp.i

CMakeFiles/Lista_02-0.0.0.dir/src/Parser.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Lista_02-0.0.0.dir/src/Parser.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/rebello/TrabalhosCBD/src/Parser.cpp -o CMakeFiles/Lista_02-0.0.0.dir/src/Parser.cpp.s

CMakeFiles/Lista_02-0.0.0.dir/src/Registro.cpp.o: CMakeFiles/Lista_02-0.0.0.dir/flags.make
CMakeFiles/Lista_02-0.0.0.dir/src/Registro.cpp.o: src/Registro.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/rebello/TrabalhosCBD/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object CMakeFiles/Lista_02-0.0.0.dir/src/Registro.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Lista_02-0.0.0.dir/src/Registro.cpp.o -c /home/rebello/TrabalhosCBD/src/Registro.cpp

CMakeFiles/Lista_02-0.0.0.dir/src/Registro.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Lista_02-0.0.0.dir/src/Registro.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/rebello/TrabalhosCBD/src/Registro.cpp > CMakeFiles/Lista_02-0.0.0.dir/src/Registro.cpp.i

CMakeFiles/Lista_02-0.0.0.dir/src/Registro.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Lista_02-0.0.0.dir/src/Registro.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/rebello/TrabalhosCBD/src/Registro.cpp -o CMakeFiles/Lista_02-0.0.0.dir/src/Registro.cpp.s

CMakeFiles/Lista_02-0.0.0.dir/src/Util.cpp.o: CMakeFiles/Lista_02-0.0.0.dir/flags.make
CMakeFiles/Lista_02-0.0.0.dir/src/Util.cpp.o: src/Util.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/rebello/TrabalhosCBD/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building CXX object CMakeFiles/Lista_02-0.0.0.dir/src/Util.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Lista_02-0.0.0.dir/src/Util.cpp.o -c /home/rebello/TrabalhosCBD/src/Util.cpp

CMakeFiles/Lista_02-0.0.0.dir/src/Util.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Lista_02-0.0.0.dir/src/Util.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/rebello/TrabalhosCBD/src/Util.cpp > CMakeFiles/Lista_02-0.0.0.dir/src/Util.cpp.i

CMakeFiles/Lista_02-0.0.0.dir/src/Util.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Lista_02-0.0.0.dir/src/Util.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/rebello/TrabalhosCBD/src/Util.cpp -o CMakeFiles/Lista_02-0.0.0.dir/src/Util.cpp.s

# Object files for target Lista_02-0.0.0
Lista_02__0_0_0_OBJECTS = \
"CMakeFiles/Lista_02-0.0.0.dir/src/main.cpp.o" \
"CMakeFiles/Lista_02-0.0.0.dir/src/Data.cpp.o" \
"CMakeFiles/Lista_02-0.0.0.dir/src/BlockIO.cpp.o" \
"CMakeFiles/Lista_02-0.0.0.dir/src/Parser.cpp.o" \
"CMakeFiles/Lista_02-0.0.0.dir/src/Registro.cpp.o" \
"CMakeFiles/Lista_02-0.0.0.dir/src/Util.cpp.o"

# External object files for target Lista_02-0.0.0
Lista_02__0_0_0_EXTERNAL_OBJECTS =

Lista_02-0.0.0-0: CMakeFiles/Lista_02-0.0.0.dir/src/main.cpp.o
Lista_02-0.0.0-0: CMakeFiles/Lista_02-0.0.0.dir/src/Data.cpp.o
Lista_02-0.0.0-0: CMakeFiles/Lista_02-0.0.0.dir/src/BlockIO.cpp.o
Lista_02-0.0.0-0: CMakeFiles/Lista_02-0.0.0.dir/src/Parser.cpp.o
Lista_02-0.0.0-0: CMakeFiles/Lista_02-0.0.0.dir/src/Registro.cpp.o
Lista_02-0.0.0-0: CMakeFiles/Lista_02-0.0.0.dir/src/Util.cpp.o
Lista_02-0.0.0-0: CMakeFiles/Lista_02-0.0.0.dir/build.make
Lista_02-0.0.0-0: lib/vhdf-1.0.0/libvhdf.so.1.0.0
Lista_02-0.0.0-0: CMakeFiles/Lista_02-0.0.0.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/rebello/TrabalhosCBD/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Linking CXX executable Lista_02-0.0.0"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/Lista_02-0.0.0.dir/link.txt --verbose=$(VERBOSE)
	$(CMAKE_COMMAND) -E cmake_symlink_executable Lista_02-0.0.0-0 Lista_02-0.0.0

Lista_02-0.0.0: Lista_02-0.0.0-0


# Rule to build all files generated by this target.
CMakeFiles/Lista_02-0.0.0.dir/build: Lista_02-0.0.0

.PHONY : CMakeFiles/Lista_02-0.0.0.dir/build

CMakeFiles/Lista_02-0.0.0.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/Lista_02-0.0.0.dir/cmake_clean.cmake
.PHONY : CMakeFiles/Lista_02-0.0.0.dir/clean

CMakeFiles/Lista_02-0.0.0.dir/depend:
	cd /home/rebello/TrabalhosCBD && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/rebello/TrabalhosCBD /home/rebello/TrabalhosCBD /home/rebello/TrabalhosCBD /home/rebello/TrabalhosCBD /home/rebello/TrabalhosCBD/CMakeFiles/Lista_02-0.0.0.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/Lista_02-0.0.0.dir/depend
