# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.14

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
CMAKE_COMMAND = "/Users/chfang/Library/Application Support/JetBrains/Toolbox/apps/CLion/ch-0/191.7141.37/CLion.app/Contents/bin/cmake/mac/bin/cmake"

# The command to remove a file.
RM = "/Users/chfang/Library/Application Support/JetBrains/Toolbox/apps/CLion/ch-0/191.7141.37/CLion.app/Contents/bin/cmake/mac/bin/cmake" -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/chfang/Documents/branch_predictor

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/chfang/Documents/branch_predictor/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/branch_predictor.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/branch_predictor.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/branch_predictor.dir/flags.make

CMakeFiles/branch_predictor.dir/src/main.c.o: CMakeFiles/branch_predictor.dir/flags.make
CMakeFiles/branch_predictor.dir/src/main.c.o: ../src/main.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/chfang/Documents/branch_predictor/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/branch_predictor.dir/src/main.c.o"
	/Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/branch_predictor.dir/src/main.c.o   -c /Users/chfang/Documents/branch_predictor/src/main.c

CMakeFiles/branch_predictor.dir/src/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/branch_predictor.dir/src/main.c.i"
	/Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/chfang/Documents/branch_predictor/src/main.c > CMakeFiles/branch_predictor.dir/src/main.c.i

CMakeFiles/branch_predictor.dir/src/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/branch_predictor.dir/src/main.c.s"
	/Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/chfang/Documents/branch_predictor/src/main.c -o CMakeFiles/branch_predictor.dir/src/main.c.s

CMakeFiles/branch_predictor.dir/src/predictor.c.o: CMakeFiles/branch_predictor.dir/flags.make
CMakeFiles/branch_predictor.dir/src/predictor.c.o: ../src/predictor.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/chfang/Documents/branch_predictor/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/branch_predictor.dir/src/predictor.c.o"
	/Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/branch_predictor.dir/src/predictor.c.o   -c /Users/chfang/Documents/branch_predictor/src/predictor.c

CMakeFiles/branch_predictor.dir/src/predictor.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/branch_predictor.dir/src/predictor.c.i"
	/Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/chfang/Documents/branch_predictor/src/predictor.c > CMakeFiles/branch_predictor.dir/src/predictor.c.i

CMakeFiles/branch_predictor.dir/src/predictor.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/branch_predictor.dir/src/predictor.c.s"
	/Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/chfang/Documents/branch_predictor/src/predictor.c -o CMakeFiles/branch_predictor.dir/src/predictor.c.s

# Object files for target branch_predictor
branch_predictor_OBJECTS = \
"CMakeFiles/branch_predictor.dir/src/main.c.o" \
"CMakeFiles/branch_predictor.dir/src/predictor.c.o"

# External object files for target branch_predictor
branch_predictor_EXTERNAL_OBJECTS =

branch_predictor: CMakeFiles/branch_predictor.dir/src/main.c.o
branch_predictor: CMakeFiles/branch_predictor.dir/src/predictor.c.o
branch_predictor: CMakeFiles/branch_predictor.dir/build.make
branch_predictor: CMakeFiles/branch_predictor.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/chfang/Documents/branch_predictor/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking C executable branch_predictor"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/branch_predictor.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/branch_predictor.dir/build: branch_predictor

.PHONY : CMakeFiles/branch_predictor.dir/build

CMakeFiles/branch_predictor.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/branch_predictor.dir/cmake_clean.cmake
.PHONY : CMakeFiles/branch_predictor.dir/clean

CMakeFiles/branch_predictor.dir/depend:
	cd /Users/chfang/Documents/branch_predictor/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/chfang/Documents/branch_predictor /Users/chfang/Documents/branch_predictor /Users/chfang/Documents/branch_predictor/cmake-build-debug /Users/chfang/Documents/branch_predictor/cmake-build-debug /Users/chfang/Documents/branch_predictor/cmake-build-debug/CMakeFiles/branch_predictor.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/branch_predictor.dir/depend

