# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.15

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
CMAKE_COMMAND = /usr/local/Cellar/cmake/3.15.5/bin/cmake

# The command to remove a file.
RM = /usr/local/Cellar/cmake/3.15.5/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/enki/Work/cmake/DataSample

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/enki/Work/cmake/DataSample/build

# Include any dependencies generated for this target.
include units/CMakeFiles/data_size.dir/depend.make

# Include the progress variables for this target.
include units/CMakeFiles/data_size.dir/progress.make

# Include the compile flags for this target's objects.
include units/CMakeFiles/data_size.dir/flags.make

units/CMakeFiles/data_size.dir/data_size.cc.o: units/CMakeFiles/data_size.dir/flags.make
units/CMakeFiles/data_size.dir/data_size.cc.o: ../units/data_size.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/enki/Work/cmake/DataSample/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object units/CMakeFiles/data_size.dir/data_size.cc.o"
	cd /Users/enki/Work/cmake/DataSample/build/units && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/data_size.dir/data_size.cc.o -c /Users/enki/Work/cmake/DataSample/units/data_size.cc

units/CMakeFiles/data_size.dir/data_size.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/data_size.dir/data_size.cc.i"
	cd /Users/enki/Work/cmake/DataSample/build/units && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/enki/Work/cmake/DataSample/units/data_size.cc > CMakeFiles/data_size.dir/data_size.cc.i

units/CMakeFiles/data_size.dir/data_size.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/data_size.dir/data_size.cc.s"
	cd /Users/enki/Work/cmake/DataSample/build/units && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/enki/Work/cmake/DataSample/units/data_size.cc -o CMakeFiles/data_size.dir/data_size.cc.s

# Object files for target data_size
data_size_OBJECTS = \
"CMakeFiles/data_size.dir/data_size.cc.o"

# External object files for target data_size
data_size_EXTERNAL_OBJECTS =

units/libdata_size.a: units/CMakeFiles/data_size.dir/data_size.cc.o
units/libdata_size.a: units/CMakeFiles/data_size.dir/build.make
units/libdata_size.a: units/CMakeFiles/data_size.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/enki/Work/cmake/DataSample/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX static library libdata_size.a"
	cd /Users/enki/Work/cmake/DataSample/build/units && $(CMAKE_COMMAND) -P CMakeFiles/data_size.dir/cmake_clean_target.cmake
	cd /Users/enki/Work/cmake/DataSample/build/units && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/data_size.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
units/CMakeFiles/data_size.dir/build: units/libdata_size.a

.PHONY : units/CMakeFiles/data_size.dir/build

units/CMakeFiles/data_size.dir/clean:
	cd /Users/enki/Work/cmake/DataSample/build/units && $(CMAKE_COMMAND) -P CMakeFiles/data_size.dir/cmake_clean.cmake
.PHONY : units/CMakeFiles/data_size.dir/clean

units/CMakeFiles/data_size.dir/depend:
	cd /Users/enki/Work/cmake/DataSample/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/enki/Work/cmake/DataSample /Users/enki/Work/cmake/DataSample/units /Users/enki/Work/cmake/DataSample/build /Users/enki/Work/cmake/DataSample/build/units /Users/enki/Work/cmake/DataSample/build/units/CMakeFiles/data_size.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : units/CMakeFiles/data_size.dir/depend

