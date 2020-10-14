# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.18

# Default target executed when no arguments are given to make.
default_target: all

.PHONY : default_target

# Allow only one "make -f Makefile2" at a time, but pass parallelism.
.NOTPARALLEL:


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
CMAKE_SOURCE_DIR = /home/destoer/projects/mina-asm

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/destoer/projects/mina-asm

#=============================================================================
# Targets provided globally by CMake.

# Special rule for the target rebuild_cache
rebuild_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Running CMake to regenerate build system..."
	/usr/bin/cmake --regenerate-during-build -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : rebuild_cache

# Special rule for the target rebuild_cache
rebuild_cache/fast: rebuild_cache

.PHONY : rebuild_cache/fast

# Special rule for the target edit_cache
edit_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Running CMake cache editor..."
	/usr/bin/ccmake -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : edit_cache

# Special rule for the target edit_cache
edit_cache/fast: edit_cache

.PHONY : edit_cache/fast

# The main all target
all: cmake_check_build_system
	$(CMAKE_COMMAND) -E cmake_progress_start /home/destoer/projects/mina-asm/CMakeFiles /home/destoer/projects/mina-asm//CMakeFiles/progress.marks
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 all
	$(CMAKE_COMMAND) -E cmake_progress_start /home/destoer/projects/mina-asm/CMakeFiles 0
.PHONY : all

# The main clean target
clean:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 clean
.PHONY : clean

# The main clean target
clean/fast: clean

.PHONY : clean/fast

# Prepare targets for installation.
preinstall: all
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall

# Prepare targets for installation.
preinstall/fast:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall/fast

# clear depends
depend:
	$(CMAKE_COMMAND) -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 1
.PHONY : depend

#=============================================================================
# Target rules for targets named mina-asm

# Build rule for target.
mina-asm: cmake_check_build_system
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 mina-asm
.PHONY : mina-asm

# fast build rule for target.
mina-asm/fast:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/mina-asm.dir/build.make CMakeFiles/mina-asm.dir/build
.PHONY : mina-asm/fast

src/main.o: src/main.cpp.o

.PHONY : src/main.o

# target to build an object file
src/main.cpp.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/mina-asm.dir/build.make CMakeFiles/mina-asm.dir/src/main.cpp.o
.PHONY : src/main.cpp.o

src/main.i: src/main.cpp.i

.PHONY : src/main.i

# target to preprocess a source file
src/main.cpp.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/mina-asm.dir/build.make CMakeFiles/mina-asm.dir/src/main.cpp.i
.PHONY : src/main.cpp.i

src/main.s: src/main.cpp.s

.PHONY : src/main.s

# target to generate assembly for a file
src/main.cpp.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/mina-asm.dir/build.make CMakeFiles/mina-asm.dir/src/main.cpp.s
.PHONY : src/main.cpp.s

src/mina/assembler.o: src/mina/assembler.cpp.o

.PHONY : src/mina/assembler.o

# target to build an object file
src/mina/assembler.cpp.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/mina-asm.dir/build.make CMakeFiles/mina-asm.dir/src/mina/assembler.cpp.o
.PHONY : src/mina/assembler.cpp.o

src/mina/assembler.i: src/mina/assembler.cpp.i

.PHONY : src/mina/assembler.i

# target to preprocess a source file
src/mina/assembler.cpp.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/mina-asm.dir/build.make CMakeFiles/mina-asm.dir/src/mina/assembler.cpp.i
.PHONY : src/mina/assembler.cpp.i

src/mina/assembler.s: src/mina/assembler.cpp.s

.PHONY : src/mina/assembler.s

# target to generate assembly for a file
src/mina/assembler.cpp.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/mina-asm.dir/build.make CMakeFiles/mina-asm.dir/src/mina/assembler.cpp.s
.PHONY : src/mina/assembler.cpp.s

src/mina/directive.o: src/mina/directive.cpp.o

.PHONY : src/mina/directive.o

# target to build an object file
src/mina/directive.cpp.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/mina-asm.dir/build.make CMakeFiles/mina-asm.dir/src/mina/directive.cpp.o
.PHONY : src/mina/directive.cpp.o

src/mina/directive.i: src/mina/directive.cpp.i

.PHONY : src/mina/directive.i

# target to preprocess a source file
src/mina/directive.cpp.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/mina-asm.dir/build.make CMakeFiles/mina-asm.dir/src/mina/directive.cpp.i
.PHONY : src/mina/directive.cpp.i

src/mina/directive.s: src/mina/directive.cpp.s

.PHONY : src/mina/directive.s

# target to generate assembly for a file
src/mina/directive.cpp.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/mina-asm.dir/build.make CMakeFiles/mina-asm.dir/src/mina/directive.cpp.s
.PHONY : src/mina/directive.cpp.s

src/mina/immediate.o: src/mina/immediate.cpp.o

.PHONY : src/mina/immediate.o

# target to build an object file
src/mina/immediate.cpp.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/mina-asm.dir/build.make CMakeFiles/mina-asm.dir/src/mina/immediate.cpp.o
.PHONY : src/mina/immediate.cpp.o

src/mina/immediate.i: src/mina/immediate.cpp.i

.PHONY : src/mina/immediate.i

# target to preprocess a source file
src/mina/immediate.cpp.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/mina-asm.dir/build.make CMakeFiles/mina-asm.dir/src/mina/immediate.cpp.i
.PHONY : src/mina/immediate.cpp.i

src/mina/immediate.s: src/mina/immediate.cpp.s

.PHONY : src/mina/immediate.s

# target to generate assembly for a file
src/mina/immediate.cpp.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/mina-asm.dir/build.make CMakeFiles/mina-asm.dir/src/mina/immediate.cpp.s
.PHONY : src/mina/immediate.cpp.s

src/mina/instr.o: src/mina/instr.cpp.o

.PHONY : src/mina/instr.o

# target to build an object file
src/mina/instr.cpp.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/mina-asm.dir/build.make CMakeFiles/mina-asm.dir/src/mina/instr.cpp.o
.PHONY : src/mina/instr.cpp.o

src/mina/instr.i: src/mina/instr.cpp.i

.PHONY : src/mina/instr.i

# target to preprocess a source file
src/mina/instr.cpp.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/mina-asm.dir/build.make CMakeFiles/mina-asm.dir/src/mina/instr.cpp.i
.PHONY : src/mina/instr.cpp.i

src/mina/instr.s: src/mina/instr.cpp.s

.PHONY : src/mina/instr.s

# target to generate assembly for a file
src/mina/instr.cpp.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/mina-asm.dir/build.make CMakeFiles/mina-asm.dir/src/mina/instr.cpp.s
.PHONY : src/mina/instr.cpp.s

src/test/imm_operand.o: src/test/imm_operand.cpp.o

.PHONY : src/test/imm_operand.o

# target to build an object file
src/test/imm_operand.cpp.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/mina-asm.dir/build.make CMakeFiles/mina-asm.dir/src/test/imm_operand.cpp.o
.PHONY : src/test/imm_operand.cpp.o

src/test/imm_operand.i: src/test/imm_operand.cpp.i

.PHONY : src/test/imm_operand.i

# target to preprocess a source file
src/test/imm_operand.cpp.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/mina-asm.dir/build.make CMakeFiles/mina-asm.dir/src/test/imm_operand.cpp.i
.PHONY : src/test/imm_operand.cpp.i

src/test/imm_operand.s: src/test/imm_operand.cpp.s

.PHONY : src/test/imm_operand.s

# target to generate assembly for a file
src/test/imm_operand.cpp.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/mina-asm.dir/build.make CMakeFiles/mina-asm.dir/src/test/imm_operand.cpp.s
.PHONY : src/test/imm_operand.cpp.s

src/test/instr.o: src/test/instr.cpp.o

.PHONY : src/test/instr.o

# target to build an object file
src/test/instr.cpp.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/mina-asm.dir/build.make CMakeFiles/mina-asm.dir/src/test/instr.cpp.o
.PHONY : src/test/instr.cpp.o

src/test/instr.i: src/test/instr.cpp.i

.PHONY : src/test/instr.i

# target to preprocess a source file
src/test/instr.cpp.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/mina-asm.dir/build.make CMakeFiles/mina-asm.dir/src/test/instr.cpp.i
.PHONY : src/test/instr.cpp.i

src/test/instr.s: src/test/instr.cpp.s

.PHONY : src/test/instr.s

# target to generate assembly for a file
src/test/instr.cpp.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/mina-asm.dir/build.make CMakeFiles/mina-asm.dir/src/test/instr.cpp.s
.PHONY : src/test/instr.cpp.s

src/test/test.o: src/test/test.cpp.o

.PHONY : src/test/test.o

# target to build an object file
src/test/test.cpp.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/mina-asm.dir/build.make CMakeFiles/mina-asm.dir/src/test/test.cpp.o
.PHONY : src/test/test.cpp.o

src/test/test.i: src/test/test.cpp.i

.PHONY : src/test/test.i

# target to preprocess a source file
src/test/test.cpp.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/mina-asm.dir/build.make CMakeFiles/mina-asm.dir/src/test/test.cpp.i
.PHONY : src/test/test.cpp.i

src/test/test.s: src/test/test.cpp.s

.PHONY : src/test/test.s

# target to generate assembly for a file
src/test/test.cpp.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/mina-asm.dir/build.make CMakeFiles/mina-asm.dir/src/test/test.cpp.s
.PHONY : src/test/test.cpp.s

# Help Target
help:
	@echo "The following are some of the valid targets for this Makefile:"
	@echo "... all (the default if no target is provided)"
	@echo "... clean"
	@echo "... depend"
	@echo "... edit_cache"
	@echo "... rebuild_cache"
	@echo "... mina-asm"
	@echo "... src/main.o"
	@echo "... src/main.i"
	@echo "... src/main.s"
	@echo "... src/mina/assembler.o"
	@echo "... src/mina/assembler.i"
	@echo "... src/mina/assembler.s"
	@echo "... src/mina/directive.o"
	@echo "... src/mina/directive.i"
	@echo "... src/mina/directive.s"
	@echo "... src/mina/immediate.o"
	@echo "... src/mina/immediate.i"
	@echo "... src/mina/immediate.s"
	@echo "... src/mina/instr.o"
	@echo "... src/mina/instr.i"
	@echo "... src/mina/instr.s"
	@echo "... src/test/imm_operand.o"
	@echo "... src/test/imm_operand.i"
	@echo "... src/test/imm_operand.s"
	@echo "... src/test/instr.o"
	@echo "... src/test/instr.i"
	@echo "... src/test/instr.s"
	@echo "... src/test/test.o"
	@echo "... src/test/test.i"
	@echo "... src/test/test.s"
.PHONY : help



#=============================================================================
# Special targets to cleanup operation of make.

# Special rule to run CMake to check the build system integrity.
# No rule that depends on this can have commands that come from listfiles
# because they might be regenerated.
cmake_check_build_system:
	$(CMAKE_COMMAND) -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 0
.PHONY : cmake_check_build_system

