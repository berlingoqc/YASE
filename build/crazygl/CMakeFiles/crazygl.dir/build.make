# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.12

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/wq/Téléchargements/YASE

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/wq/Téléchargements/YASE/build

# Include any dependencies generated for this target.
include crazygl/CMakeFiles/crazygl.dir/depend.make

# Include the progress variables for this target.
include crazygl/CMakeFiles/crazygl.dir/progress.make

# Include the compile flags for this target's objects.
include crazygl/CMakeFiles/crazygl.dir/flags.make

crazygl/CMakeFiles/crazygl.dir/shaders.cpp.o: crazygl/CMakeFiles/crazygl.dir/flags.make
crazygl/CMakeFiles/crazygl.dir/shaders.cpp.o: ../crazygl/shaders.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/wq/Téléchargements/YASE/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object crazygl/CMakeFiles/crazygl.dir/shaders.cpp.o"
	cd /home/wq/Téléchargements/YASE/build/crazygl && /usr/bin/clang++-7  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/crazygl.dir/shaders.cpp.o -c /home/wq/Téléchargements/YASE/crazygl/shaders.cpp

crazygl/CMakeFiles/crazygl.dir/shaders.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/crazygl.dir/shaders.cpp.i"
	cd /home/wq/Téléchargements/YASE/build/crazygl && /usr/bin/clang++-7 $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/wq/Téléchargements/YASE/crazygl/shaders.cpp > CMakeFiles/crazygl.dir/shaders.cpp.i

crazygl/CMakeFiles/crazygl.dir/shaders.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/crazygl.dir/shaders.cpp.s"
	cd /home/wq/Téléchargements/YASE/build/crazygl && /usr/bin/clang++-7 $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/wq/Téléchargements/YASE/crazygl/shaders.cpp -o CMakeFiles/crazygl.dir/shaders.cpp.s

crazygl/CMakeFiles/crazygl.dir/textures.cpp.o: crazygl/CMakeFiles/crazygl.dir/flags.make
crazygl/CMakeFiles/crazygl.dir/textures.cpp.o: ../crazygl/textures.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/wq/Téléchargements/YASE/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object crazygl/CMakeFiles/crazygl.dir/textures.cpp.o"
	cd /home/wq/Téléchargements/YASE/build/crazygl && /usr/bin/clang++-7  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/crazygl.dir/textures.cpp.o -c /home/wq/Téléchargements/YASE/crazygl/textures.cpp

crazygl/CMakeFiles/crazygl.dir/textures.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/crazygl.dir/textures.cpp.i"
	cd /home/wq/Téléchargements/YASE/build/crazygl && /usr/bin/clang++-7 $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/wq/Téléchargements/YASE/crazygl/textures.cpp > CMakeFiles/crazygl.dir/textures.cpp.i

crazygl/CMakeFiles/crazygl.dir/textures.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/crazygl.dir/textures.cpp.s"
	cd /home/wq/Téléchargements/YASE/build/crazygl && /usr/bin/clang++-7 $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/wq/Téléchargements/YASE/crazygl/textures.cpp -o CMakeFiles/crazygl.dir/textures.cpp.s

# Object files for target crazygl
crazygl_OBJECTS = \
"CMakeFiles/crazygl.dir/shaders.cpp.o" \
"CMakeFiles/crazygl.dir/textures.cpp.o"

# External object files for target crazygl
crazygl_EXTERNAL_OBJECTS =

crazygl/libcrazygl.a: crazygl/CMakeFiles/crazygl.dir/shaders.cpp.o
crazygl/libcrazygl.a: crazygl/CMakeFiles/crazygl.dir/textures.cpp.o
crazygl/libcrazygl.a: crazygl/CMakeFiles/crazygl.dir/build.make
crazygl/libcrazygl.a: crazygl/CMakeFiles/crazygl.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/wq/Téléchargements/YASE/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX static library libcrazygl.a"
	cd /home/wq/Téléchargements/YASE/build/crazygl && $(CMAKE_COMMAND) -P CMakeFiles/crazygl.dir/cmake_clean_target.cmake
	cd /home/wq/Téléchargements/YASE/build/crazygl && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/crazygl.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
crazygl/CMakeFiles/crazygl.dir/build: crazygl/libcrazygl.a

.PHONY : crazygl/CMakeFiles/crazygl.dir/build

crazygl/CMakeFiles/crazygl.dir/clean:
	cd /home/wq/Téléchargements/YASE/build/crazygl && $(CMAKE_COMMAND) -P CMakeFiles/crazygl.dir/cmake_clean.cmake
.PHONY : crazygl/CMakeFiles/crazygl.dir/clean

crazygl/CMakeFiles/crazygl.dir/depend:
	cd /home/wq/Téléchargements/YASE/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/wq/Téléchargements/YASE /home/wq/Téléchargements/YASE/crazygl /home/wq/Téléchargements/YASE/build /home/wq/Téléchargements/YASE/build/crazygl /home/wq/Téléchargements/YASE/build/crazygl/CMakeFiles/crazygl.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : crazygl/CMakeFiles/crazygl.dir/depend
