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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/jojo/current_focus/public/L19_Broflovski

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/jojo/current_focus/public/L19_Broflovski/src

# Include any dependencies generated for this target.
include src/CMakeFiles/broflovski.dir/depend.make

# Include the progress variables for this target.
include src/CMakeFiles/broflovski.dir/progress.make

# Include the compile flags for this target's objects.
include src/CMakeFiles/broflovski.dir/flags.make

src/CMakeFiles/broflovski.dir/broflovski.c.o: src/CMakeFiles/broflovski.dir/flags.make
src/CMakeFiles/broflovski.dir/broflovski.c.o: broflovski.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/jojo/current_focus/public/L19_Broflovski/src/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object src/CMakeFiles/broflovski.dir/broflovski.c.o"
	cd /home/jojo/current_focus/public/L19_Broflovski/src/src && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/broflovski.dir/broflovski.c.o   -c /home/jojo/current_focus/public/L19_Broflovski/src/broflovski.c

src/CMakeFiles/broflovski.dir/broflovski.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/broflovski.dir/broflovski.c.i"
	cd /home/jojo/current_focus/public/L19_Broflovski/src/src && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/jojo/current_focus/public/L19_Broflovski/src/broflovski.c > CMakeFiles/broflovski.dir/broflovski.c.i

src/CMakeFiles/broflovski.dir/broflovski.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/broflovski.dir/broflovski.c.s"
	cd /home/jojo/current_focus/public/L19_Broflovski/src/src && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/jojo/current_focus/public/L19_Broflovski/src/broflovski.c -o CMakeFiles/broflovski.dir/broflovski.c.s

# Object files for target broflovski
broflovski_OBJECTS = \
"CMakeFiles/broflovski.dir/broflovski.c.o"

# External object files for target broflovski
broflovski_EXTERNAL_OBJECTS =

src/libbroflovski.a: src/CMakeFiles/broflovski.dir/broflovski.c.o
src/libbroflovski.a: src/CMakeFiles/broflovski.dir/build.make
src/libbroflovski.a: src/CMakeFiles/broflovski.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/jojo/current_focus/public/L19_Broflovski/src/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C static library libbroflovski.a"
	cd /home/jojo/current_focus/public/L19_Broflovski/src/src && $(CMAKE_COMMAND) -P CMakeFiles/broflovski.dir/cmake_clean_target.cmake
	cd /home/jojo/current_focus/public/L19_Broflovski/src/src && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/broflovski.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/CMakeFiles/broflovski.dir/build: src/libbroflovski.a

.PHONY : src/CMakeFiles/broflovski.dir/build

src/CMakeFiles/broflovski.dir/clean:
	cd /home/jojo/current_focus/public/L19_Broflovski/src/src && $(CMAKE_COMMAND) -P CMakeFiles/broflovski.dir/cmake_clean.cmake
.PHONY : src/CMakeFiles/broflovski.dir/clean

src/CMakeFiles/broflovski.dir/depend:
	cd /home/jojo/current_focus/public/L19_Broflovski/src && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/jojo/current_focus/public/L19_Broflovski /home/jojo/current_focus/public/L19_Broflovski/src /home/jojo/current_focus/public/L19_Broflovski/src /home/jojo/current_focus/public/L19_Broflovski/src/src /home/jojo/current_focus/public/L19_Broflovski/src/src/CMakeFiles/broflovski.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/CMakeFiles/broflovski.dir/depend

