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
CMAKE_COMMAND = /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake

# The command to remove a file.
RM = /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/vumilan/CLionProjects/PA2/PA2-progtests/07

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/vumilan/CLionProjects/PA2/PA2-progtests/07/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/07.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/07.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/07.dir/flags.make

CMakeFiles/07.dir/main.cpp.o: CMakeFiles/07.dir/flags.make
CMakeFiles/07.dir/main.cpp.o: ../main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/vumilan/CLionProjects/PA2/PA2-progtests/07/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/07.dir/main.cpp.o"
	/Library/Developer/CommandLineTools/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/07.dir/main.cpp.o -c /Users/vumilan/CLionProjects/PA2/PA2-progtests/07/main.cpp

CMakeFiles/07.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/07.dir/main.cpp.i"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/vumilan/CLionProjects/PA2/PA2-progtests/07/main.cpp > CMakeFiles/07.dir/main.cpp.i

CMakeFiles/07.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/07.dir/main.cpp.s"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/vumilan/CLionProjects/PA2/PA2-progtests/07/main.cpp -o CMakeFiles/07.dir/main.cpp.s

# Object files for target 07
07_OBJECTS = \
"CMakeFiles/07.dir/main.cpp.o"

# External object files for target 07
07_EXTERNAL_OBJECTS =

07: CMakeFiles/07.dir/main.cpp.o
07: CMakeFiles/07.dir/build.make
07: CMakeFiles/07.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/vumilan/CLionProjects/PA2/PA2-progtests/07/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable 07"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/07.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/07.dir/build: 07

.PHONY : CMakeFiles/07.dir/build

CMakeFiles/07.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/07.dir/cmake_clean.cmake
.PHONY : CMakeFiles/07.dir/clean

CMakeFiles/07.dir/depend:
	cd /Users/vumilan/CLionProjects/PA2/PA2-progtests/07/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/vumilan/CLionProjects/PA2/PA2-progtests/07 /Users/vumilan/CLionProjects/PA2/PA2-progtests/07 /Users/vumilan/CLionProjects/PA2/PA2-progtests/07/cmake-build-debug /Users/vumilan/CLionProjects/PA2/PA2-progtests/07/cmake-build-debug /Users/vumilan/CLionProjects/PA2/PA2-progtests/07/cmake-build-debug/CMakeFiles/07.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/07.dir/depend

