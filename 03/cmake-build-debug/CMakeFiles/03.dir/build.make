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
CMAKE_SOURCE_DIR = /Users/vumilan/CLionProjects/PA2/PA2-progtests/03

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/vumilan/CLionProjects/PA2/PA2-progtests/03/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/03.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/03.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/03.dir/flags.make

CMakeFiles/03.dir/main.cpp.o: CMakeFiles/03.dir/flags.make
CMakeFiles/03.dir/main.cpp.o: ../main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/vumilan/CLionProjects/PA2/PA2-progtests/03/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/03.dir/main.cpp.o"
	/Library/Developer/CommandLineTools/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/03.dir/main.cpp.o -c /Users/vumilan/CLionProjects/PA2/PA2-progtests/03/main.cpp

CMakeFiles/03.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/03.dir/main.cpp.i"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/vumilan/CLionProjects/PA2/PA2-progtests/03/main.cpp > CMakeFiles/03.dir/main.cpp.i

CMakeFiles/03.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/03.dir/main.cpp.s"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/vumilan/CLionProjects/PA2/PA2-progtests/03/main.cpp -o CMakeFiles/03.dir/main.cpp.s

# Object files for target 03
03_OBJECTS = \
"CMakeFiles/03.dir/main.cpp.o"

# External object files for target 03
03_EXTERNAL_OBJECTS =

03: CMakeFiles/03.dir/main.cpp.o
03: CMakeFiles/03.dir/build.make
03: CMakeFiles/03.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/vumilan/CLionProjects/PA2/PA2-progtests/03/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable 03"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/03.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/03.dir/build: 03

.PHONY : CMakeFiles/03.dir/build

CMakeFiles/03.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/03.dir/cmake_clean.cmake
.PHONY : CMakeFiles/03.dir/clean

CMakeFiles/03.dir/depend:
	cd /Users/vumilan/CLionProjects/PA2/PA2-progtests/03/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/vumilan/CLionProjects/PA2/PA2-progtests/03 /Users/vumilan/CLionProjects/PA2/PA2-progtests/03 /Users/vumilan/CLionProjects/PA2/PA2-progtests/03/cmake-build-debug /Users/vumilan/CLionProjects/PA2/PA2-progtests/03/cmake-build-debug /Users/vumilan/CLionProjects/PA2/PA2-progtests/03/cmake-build-debug/CMakeFiles/03.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/03.dir/depend

