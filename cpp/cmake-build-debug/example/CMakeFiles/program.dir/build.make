# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
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
RM = /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/shenheping/cpp-sdk/cpp

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/shenheping/cpp-sdk/cpp/cmake-build-debug

# Include any dependencies generated for this target.
include example/CMakeFiles/program.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include example/CMakeFiles/program.dir/compiler_depend.make

# Include the progress variables for this target.
include example/CMakeFiles/program.dir/progress.make

# Include the compile flags for this target's objects.
include example/CMakeFiles/program.dir/flags.make

example/CMakeFiles/program.dir/program.cpp.o: example/CMakeFiles/program.dir/flags.make
example/CMakeFiles/program.dir/program.cpp.o: ../example/program.cpp
example/CMakeFiles/program.dir/program.cpp.o: example/CMakeFiles/program.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/shenheping/cpp-sdk/cpp/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object example/CMakeFiles/program.dir/program.cpp.o"
	cd /Users/shenheping/cpp-sdk/cpp/cmake-build-debug/example && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT example/CMakeFiles/program.dir/program.cpp.o -MF CMakeFiles/program.dir/program.cpp.o.d -o CMakeFiles/program.dir/program.cpp.o -c /Users/shenheping/cpp-sdk/cpp/example/program.cpp

example/CMakeFiles/program.dir/program.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/program.dir/program.cpp.i"
	cd /Users/shenheping/cpp-sdk/cpp/cmake-build-debug/example && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/shenheping/cpp-sdk/cpp/example/program.cpp > CMakeFiles/program.dir/program.cpp.i

example/CMakeFiles/program.dir/program.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/program.dir/program.cpp.s"
	cd /Users/shenheping/cpp-sdk/cpp/cmake-build-debug/example && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/shenheping/cpp-sdk/cpp/example/program.cpp -o CMakeFiles/program.dir/program.cpp.s

# Object files for target program
program_OBJECTS = \
"CMakeFiles/program.dir/program.cpp.o"

# External object files for target program
program_EXTERNAL_OBJECTS =

example/program: example/CMakeFiles/program.dir/program.cpp.o
example/program: example/CMakeFiles/program.dir/build.make
example/program: libtacpp.a
example/program: example/CMakeFiles/program.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/shenheping/cpp-sdk/cpp/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable program"
	cd /Users/shenheping/cpp-sdk/cpp/cmake-build-debug/example && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/program.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
example/CMakeFiles/program.dir/build: example/program
.PHONY : example/CMakeFiles/program.dir/build

example/CMakeFiles/program.dir/clean:
	cd /Users/shenheping/cpp-sdk/cpp/cmake-build-debug/example && $(CMAKE_COMMAND) -P CMakeFiles/program.dir/cmake_clean.cmake
.PHONY : example/CMakeFiles/program.dir/clean

example/CMakeFiles/program.dir/depend:
	cd /Users/shenheping/cpp-sdk/cpp/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/shenheping/cpp-sdk/cpp /Users/shenheping/cpp-sdk/cpp/example /Users/shenheping/cpp-sdk/cpp/cmake-build-debug /Users/shenheping/cpp-sdk/cpp/cmake-build-debug/example /Users/shenheping/cpp-sdk/cpp/cmake-build-debug/example/CMakeFiles/program.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : example/CMakeFiles/program.dir/depend

