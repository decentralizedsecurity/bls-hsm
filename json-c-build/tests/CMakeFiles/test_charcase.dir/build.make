# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

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
CMAKE_SOURCE_DIR = /home/jorge/bls-hsm/json-c

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/jorge/bls-hsm/json-c-build

# Include any dependencies generated for this target.
include tests/CMakeFiles/test_charcase.dir/depend.make

# Include the progress variables for this target.
include tests/CMakeFiles/test_charcase.dir/progress.make

# Include the compile flags for this target's objects.
include tests/CMakeFiles/test_charcase.dir/flags.make

tests/CMakeFiles/test_charcase.dir/test_charcase.c.o: tests/CMakeFiles/test_charcase.dir/flags.make
tests/CMakeFiles/test_charcase.dir/test_charcase.c.o: /home/jorge/bls-hsm/json-c/tests/test_charcase.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/jorge/bls-hsm/json-c-build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object tests/CMakeFiles/test_charcase.dir/test_charcase.c.o"
	cd /home/jorge/bls-hsm/json-c-build/tests && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/test_charcase.dir/test_charcase.c.o   -c /home/jorge/bls-hsm/json-c/tests/test_charcase.c

tests/CMakeFiles/test_charcase.dir/test_charcase.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/test_charcase.dir/test_charcase.c.i"
	cd /home/jorge/bls-hsm/json-c-build/tests && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/jorge/bls-hsm/json-c/tests/test_charcase.c > CMakeFiles/test_charcase.dir/test_charcase.c.i

tests/CMakeFiles/test_charcase.dir/test_charcase.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/test_charcase.dir/test_charcase.c.s"
	cd /home/jorge/bls-hsm/json-c-build/tests && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/jorge/bls-hsm/json-c/tests/test_charcase.c -o CMakeFiles/test_charcase.dir/test_charcase.c.s

# Object files for target test_charcase
test_charcase_OBJECTS = \
"CMakeFiles/test_charcase.dir/test_charcase.c.o"

# External object files for target test_charcase
test_charcase_EXTERNAL_OBJECTS =

tests/test_charcase: tests/CMakeFiles/test_charcase.dir/test_charcase.c.o
tests/test_charcase: tests/CMakeFiles/test_charcase.dir/build.make
tests/test_charcase: libjson-c.so.5.1.0
tests/test_charcase: tests/CMakeFiles/test_charcase.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/jorge/bls-hsm/json-c-build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable test_charcase"
	cd /home/jorge/bls-hsm/json-c-build/tests && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/test_charcase.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
tests/CMakeFiles/test_charcase.dir/build: tests/test_charcase

.PHONY : tests/CMakeFiles/test_charcase.dir/build

tests/CMakeFiles/test_charcase.dir/clean:
	cd /home/jorge/bls-hsm/json-c-build/tests && $(CMAKE_COMMAND) -P CMakeFiles/test_charcase.dir/cmake_clean.cmake
.PHONY : tests/CMakeFiles/test_charcase.dir/clean

tests/CMakeFiles/test_charcase.dir/depend:
	cd /home/jorge/bls-hsm/json-c-build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/jorge/bls-hsm/json-c /home/jorge/bls-hsm/json-c/tests /home/jorge/bls-hsm/json-c-build /home/jorge/bls-hsm/json-c-build/tests /home/jorge/bls-hsm/json-c-build/tests/CMakeFiles/test_charcase.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : tests/CMakeFiles/test_charcase.dir/depend

