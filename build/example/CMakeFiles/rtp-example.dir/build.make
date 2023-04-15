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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/newman/repos/MediaX

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/newman/repos/MediaX/build

# Include any dependencies generated for this target.
include example/CMakeFiles/rtp-example.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include example/CMakeFiles/rtp-example.dir/compiler_depend.make

# Include the progress variables for this target.
include example/CMakeFiles/rtp-example.dir/progress.make

# Include the compile flags for this target's objects.
include example/CMakeFiles/rtp-example.dir/flags.make

example/CMakeFiles/rtp-example.dir/example.cc.o: example/CMakeFiles/rtp-example.dir/flags.make
example/CMakeFiles/rtp-example.dir/example.cc.o: ../example/example.cc
example/CMakeFiles/rtp-example.dir/example.cc.o: example/CMakeFiles/rtp-example.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/newman/repos/MediaX/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object example/CMakeFiles/rtp-example.dir/example.cc.o"
	cd /home/newman/repos/MediaX/build/example && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT example/CMakeFiles/rtp-example.dir/example.cc.o -MF CMakeFiles/rtp-example.dir/example.cc.o.d -o CMakeFiles/rtp-example.dir/example.cc.o -c /home/newman/repos/MediaX/example/example.cc

example/CMakeFiles/rtp-example.dir/example.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/rtp-example.dir/example.cc.i"
	cd /home/newman/repos/MediaX/build/example && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/newman/repos/MediaX/example/example.cc > CMakeFiles/rtp-example.dir/example.cc.i

example/CMakeFiles/rtp-example.dir/example.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/rtp-example.dir/example.cc.s"
	cd /home/newman/repos/MediaX/build/example && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/newman/repos/MediaX/example/example.cc -o CMakeFiles/rtp-example.dir/example.cc.s

example/CMakeFiles/rtp-example.dir/pngget.cc.o: example/CMakeFiles/rtp-example.dir/flags.make
example/CMakeFiles/rtp-example.dir/pngget.cc.o: ../example/pngget.cc
example/CMakeFiles/rtp-example.dir/pngget.cc.o: example/CMakeFiles/rtp-example.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/newman/repos/MediaX/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object example/CMakeFiles/rtp-example.dir/pngget.cc.o"
	cd /home/newman/repos/MediaX/build/example && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT example/CMakeFiles/rtp-example.dir/pngget.cc.o -MF CMakeFiles/rtp-example.dir/pngget.cc.o.d -o CMakeFiles/rtp-example.dir/pngget.cc.o -c /home/newman/repos/MediaX/example/pngget.cc

example/CMakeFiles/rtp-example.dir/pngget.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/rtp-example.dir/pngget.cc.i"
	cd /home/newman/repos/MediaX/build/example && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/newman/repos/MediaX/example/pngget.cc > CMakeFiles/rtp-example.dir/pngget.cc.i

example/CMakeFiles/rtp-example.dir/pngget.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/rtp-example.dir/pngget.cc.s"
	cd /home/newman/repos/MediaX/build/example && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/newman/repos/MediaX/example/pngget.cc -o CMakeFiles/rtp-example.dir/pngget.cc.s

# Object files for target rtp-example
rtp__example_OBJECTS = \
"CMakeFiles/rtp-example.dir/example.cc.o" \
"CMakeFiles/rtp-example.dir/pngget.cc.o"

# External object files for target rtp-example
rtp__example_EXTERNAL_OBJECTS =

bin/rtp-example: example/CMakeFiles/rtp-example.dir/example.cc.o
bin/rtp-example: example/CMakeFiles/rtp-example.dir/pngget.cc.o
bin/rtp-example: example/CMakeFiles/rtp-example.dir/build.make
bin/rtp-example: lib/libmediax.so.1
bin/rtp-example: /usr/lib/x86_64-linux-gnu/libgflags.so.2.2.2
bin/rtp-example: example/CMakeFiles/rtp-example.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/newman/repos/MediaX/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable ../bin/rtp-example"
	cd /home/newman/repos/MediaX/build/example && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/rtp-example.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
example/CMakeFiles/rtp-example.dir/build: bin/rtp-example
.PHONY : example/CMakeFiles/rtp-example.dir/build

example/CMakeFiles/rtp-example.dir/clean:
	cd /home/newman/repos/MediaX/build/example && $(CMAKE_COMMAND) -P CMakeFiles/rtp-example.dir/cmake_clean.cmake
.PHONY : example/CMakeFiles/rtp-example.dir/clean

example/CMakeFiles/rtp-example.dir/depend:
	cd /home/newman/repos/MediaX/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/newman/repos/MediaX /home/newman/repos/MediaX/example /home/newman/repos/MediaX/build /home/newman/repos/MediaX/build/example /home/newman/repos/MediaX/build/example/CMakeFiles/rtp-example.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : example/CMakeFiles/rtp-example.dir/depend
