# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.11

# Default target executed when no arguments are given to make.
default_target: all

.PHONY : default_target

# Allow only one "make -f Makefile2" at a time, but pass parallelism.
.NOTPARALLEL:


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
CMAKE_SOURCE_DIR = /home/pawel/projects/saitama

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/pawel/projects/saitama

#=============================================================================
# Targets provided globally by CMake.

# Special rule for the target rebuild_cache
rebuild_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Running CMake to regenerate build system..."
	/usr/bin/cmake -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : rebuild_cache

# Special rule for the target rebuild_cache
rebuild_cache/fast: rebuild_cache

.PHONY : rebuild_cache/fast

# Special rule for the target edit_cache
edit_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Running CMake cache editor..."
	/usr/bin/ccmake -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : edit_cache

# Special rule for the target edit_cache
edit_cache/fast: edit_cache

.PHONY : edit_cache/fast

# The main all target
all: cmake_check_build_system
	$(CMAKE_COMMAND) -E cmake_progress_start /home/pawel/projects/saitama/CMakeFiles /home/pawel/projects/saitama/CMakeFiles/progress.marks
	$(MAKE) -f CMakeFiles/Makefile2 all
	$(CMAKE_COMMAND) -E cmake_progress_start /home/pawel/projects/saitama/CMakeFiles 0
.PHONY : all

# The main clean target
clean:
	$(MAKE) -f CMakeFiles/Makefile2 clean
.PHONY : clean

# The main clean target
clean/fast: clean

.PHONY : clean/fast

# Prepare targets for installation.
preinstall: all
	$(MAKE) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall

# Prepare targets for installation.
preinstall/fast:
	$(MAKE) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall/fast

# clear depends
depend:
	$(CMAKE_COMMAND) -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 1
.PHONY : depend

#=============================================================================
# Target rules for targets named saitama

# Build rule for target.
saitama: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 saitama
.PHONY : saitama

# fast build rule for target.
saitama/fast:
	$(MAKE) -f CMakeFiles/saitama.dir/build.make CMakeFiles/saitama.dir/build
.PHONY : saitama/fast

src/attack.o: src/attack.c.o

.PHONY : src/attack.o

# target to build an object file
src/attack.c.o:
	$(MAKE) -f CMakeFiles/saitama.dir/build.make CMakeFiles/saitama.dir/src/attack.c.o
.PHONY : src/attack.c.o

src/attack.i: src/attack.c.i

.PHONY : src/attack.i

# target to preprocess a source file
src/attack.c.i:
	$(MAKE) -f CMakeFiles/saitama.dir/build.make CMakeFiles/saitama.dir/src/attack.c.i
.PHONY : src/attack.c.i

src/attack.s: src/attack.c.s

.PHONY : src/attack.s

# target to generate assembly for a file
src/attack.c.s:
	$(MAKE) -f CMakeFiles/saitama.dir/build.make CMakeFiles/saitama.dir/src/attack.c.s
.PHONY : src/attack.c.s

src/bitboard.o: src/bitboard.c.o

.PHONY : src/bitboard.o

# target to build an object file
src/bitboard.c.o:
	$(MAKE) -f CMakeFiles/saitama.dir/build.make CMakeFiles/saitama.dir/src/bitboard.c.o
.PHONY : src/bitboard.c.o

src/bitboard.i: src/bitboard.c.i

.PHONY : src/bitboard.i

# target to preprocess a source file
src/bitboard.c.i:
	$(MAKE) -f CMakeFiles/saitama.dir/build.make CMakeFiles/saitama.dir/src/bitboard.c.i
.PHONY : src/bitboard.c.i

src/bitboard.s: src/bitboard.c.s

.PHONY : src/bitboard.s

# target to generate assembly for a file
src/bitboard.c.s:
	$(MAKE) -f CMakeFiles/saitama.dir/build.make CMakeFiles/saitama.dir/src/bitboard.c.s
.PHONY : src/bitboard.c.s

src/board.o: src/board.c.o

.PHONY : src/board.o

# target to build an object file
src/board.c.o:
	$(MAKE) -f CMakeFiles/saitama.dir/build.make CMakeFiles/saitama.dir/src/board.c.o
.PHONY : src/board.c.o

src/board.i: src/board.c.i

.PHONY : src/board.i

# target to preprocess a source file
src/board.c.i:
	$(MAKE) -f CMakeFiles/saitama.dir/build.make CMakeFiles/saitama.dir/src/board.c.i
.PHONY : src/board.c.i

src/board.s: src/board.c.s

.PHONY : src/board.s

# target to generate assembly for a file
src/board.c.s:
	$(MAKE) -f CMakeFiles/saitama.dir/build.make CMakeFiles/saitama.dir/src/board.c.s
.PHONY : src/board.c.s

src/data.o: src/data.c.o

.PHONY : src/data.o

# target to build an object file
src/data.c.o:
	$(MAKE) -f CMakeFiles/saitama.dir/build.make CMakeFiles/saitama.dir/src/data.c.o
.PHONY : src/data.c.o

src/data.i: src/data.c.i

.PHONY : src/data.i

# target to preprocess a source file
src/data.c.i:
	$(MAKE) -f CMakeFiles/saitama.dir/build.make CMakeFiles/saitama.dir/src/data.c.i
.PHONY : src/data.c.i

src/data.s: src/data.c.s

.PHONY : src/data.s

# target to generate assembly for a file
src/data.c.s:
	$(MAKE) -f CMakeFiles/saitama.dir/build.make CMakeFiles/saitama.dir/src/data.c.s
.PHONY : src/data.c.s

src/hashkeys.o: src/hashkeys.c.o

.PHONY : src/hashkeys.o

# target to build an object file
src/hashkeys.c.o:
	$(MAKE) -f CMakeFiles/saitama.dir/build.make CMakeFiles/saitama.dir/src/hashkeys.c.o
.PHONY : src/hashkeys.c.o

src/hashkeys.i: src/hashkeys.c.i

.PHONY : src/hashkeys.i

# target to preprocess a source file
src/hashkeys.c.i:
	$(MAKE) -f CMakeFiles/saitama.dir/build.make CMakeFiles/saitama.dir/src/hashkeys.c.i
.PHONY : src/hashkeys.c.i

src/hashkeys.s: src/hashkeys.c.s

.PHONY : src/hashkeys.s

# target to generate assembly for a file
src/hashkeys.c.s:
	$(MAKE) -f CMakeFiles/saitama.dir/build.make CMakeFiles/saitama.dir/src/hashkeys.c.s
.PHONY : src/hashkeys.c.s

src/init.o: src/init.c.o

.PHONY : src/init.o

# target to build an object file
src/init.c.o:
	$(MAKE) -f CMakeFiles/saitama.dir/build.make CMakeFiles/saitama.dir/src/init.c.o
.PHONY : src/init.c.o

src/init.i: src/init.c.i

.PHONY : src/init.i

# target to preprocess a source file
src/init.c.i:
	$(MAKE) -f CMakeFiles/saitama.dir/build.make CMakeFiles/saitama.dir/src/init.c.i
.PHONY : src/init.c.i

src/init.s: src/init.c.s

.PHONY : src/init.s

# target to generate assembly for a file
src/init.c.s:
	$(MAKE) -f CMakeFiles/saitama.dir/build.make CMakeFiles/saitama.dir/src/init.c.s
.PHONY : src/init.c.s

src/saitama.o: src/saitama.c.o

.PHONY : src/saitama.o

# target to build an object file
src/saitama.c.o:
	$(MAKE) -f CMakeFiles/saitama.dir/build.make CMakeFiles/saitama.dir/src/saitama.c.o
.PHONY : src/saitama.c.o

src/saitama.i: src/saitama.c.i

.PHONY : src/saitama.i

# target to preprocess a source file
src/saitama.c.i:
	$(MAKE) -f CMakeFiles/saitama.dir/build.make CMakeFiles/saitama.dir/src/saitama.c.i
.PHONY : src/saitama.c.i

src/saitama.s: src/saitama.c.s

.PHONY : src/saitama.s

# target to generate assembly for a file
src/saitama.c.s:
	$(MAKE) -f CMakeFiles/saitama.dir/build.make CMakeFiles/saitama.dir/src/saitama.c.s
.PHONY : src/saitama.c.s

# Help Target
help:
	@echo "The following are some of the valid targets for this Makefile:"
	@echo "... all (the default if no target is provided)"
	@echo "... clean"
	@echo "... depend"
	@echo "... rebuild_cache"
	@echo "... saitama"
	@echo "... edit_cache"
	@echo "... src/attack.o"
	@echo "... src/attack.i"
	@echo "... src/attack.s"
	@echo "... src/bitboard.o"
	@echo "... src/bitboard.i"
	@echo "... src/bitboard.s"
	@echo "... src/board.o"
	@echo "... src/board.i"
	@echo "... src/board.s"
	@echo "... src/data.o"
	@echo "... src/data.i"
	@echo "... src/data.s"
	@echo "... src/hashkeys.o"
	@echo "... src/hashkeys.i"
	@echo "... src/hashkeys.s"
	@echo "... src/init.o"
	@echo "... src/init.i"
	@echo "... src/init.s"
	@echo "... src/saitama.o"
	@echo "... src/saitama.i"
	@echo "... src/saitama.s"
.PHONY : help



#=============================================================================
# Special targets to cleanup operation of make.

# Special rule to run CMake to check the build system integrity.
# No rule that depends on this can have commands that come from listfiles
# because they might be regenerated.
cmake_check_build_system:
	$(CMAKE_COMMAND) -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 0
.PHONY : cmake_check_build_system

