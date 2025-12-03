OBJ_DIR := .obj
BIN_DIR := bin
THIRD_PARTY_DIR := lib

# Get absolute path for library directory
LIB_ABS_PATH := $(shell pwd)/$(THIRD_PARTY_DIR)

# Set LD_LIBRARY_PATH to include our lib directory
export LD_LIBRARY_PATH := $(LIB_ABS_PATH):$(LD_LIBRARY_PATH)

# external headers will be located under lib/include/ directory
INC_DIRS = -I$(SRC_DIR) -I$(THIRD_PARTY_DIR)/include

# GCC features
# Wall: activate default warnings
# Wpedantic: enforce for ISO C/C++ standart
# Wextra: activate non-default warnings
# Wconversion: warn about type conversions that may lead data loss (ie: int x = 13.23d) 
# Wcast-align: warn when casting violates alignment rules (ie: char* char_p = &x ) 
# Wunused: warn about unused stuffs such as unused parameters and variables
# Wuninitialized: warn about uninitialized variables
# Wsign-compare: warn about signed-unsigned comparision
# SPDLOG_COMPILED_LIB: Use spdlog as compiled library (not header-only)
# Note: ZMQ_BUILD_DRAFT_API is defined in lib/include/zmq_config.hpp
CXX := g++
CPPFLAGS = -std=c++17 $(INC_DIRS) -L$(THIRD_PARTY_DIR) -O3 -Wall -Wpedantic -Wextra -Wconversion -Wcast-align -Wunused -Wuninitialized -Wsign-compare -DSPDLOG_COMPILED_LIB -pthread

# Acquiring external .so files from lib directory (ie. libzmq.so, libspdlog.so)
SO_FILES = $(notdir $(shell find $(THIRD_PARTY_DIR) -maxdepth 1 -type f -name "*.so" -o -type l -name "*.so"))
# Removing 'lib' prefixes (ie. libzmq.so -> zmq.so)
SO_LIB_NAME = $(shell echo $(SO_FILES) | sed 's/lib//g')
# Removing .so and adding -l prefix (ie. zmq.so -> zmq -> -lzmq) 
SO_LD_FLAGS = $(shell for lib in $(SO_LIB_NAME); do echo $$lib | cut -d"." -f1 | sed 's/^/-l/'; done)

# Add pthread for threading support
LD_FLAGS = $(SO_LD_FLAGS) -lpthread

RPATH_CONFIG = -Wl,-rpath,'$$ORIGIN/../lib'

# Directories will be checked out by check_dirs target
CHECK_DIRS = $(SRC_DIR)/adapters $(SRC_DIR)/domain $(SRC_DIR)/utils \
	$(SRC_DIR)/adapters/common $(SRC_DIR)/adapters/incoming $(SRC_DIR)/adapters/outgoing \
	$(SRC_DIR)/domain/logic $(SRC_DIR)/domain/model $(SRC_DIR)/domain/ports \
	$(SRC_DIR)/domain/ports/outgoing $(SRC_DIR)/domain/ports/outgoing \
	./test ./lib ./lib/include ./bin ./doc

# Directories will be checked out by check_files target
CHECK_FILES = $(SRC_DIR)/main.cpp
	
# Acquiring source files
SRC_FILES = $(shell find $(SRC_DIR) -type f -name "*.cpp")
OBJ_FILES = $(SRC_FILES:.cpp=.o)

DASHES := "******************"

default : objs

exec : check_necessary_dirs check_necessary_files generate_executable
objs : check_necessary_dirs generate_objects

# Generating executable file under bin/ directory
generate_executable : compile_all_sources
	@echo -e "\n $(DASHES) \n  GENERATING EXECUTABLE $(EXECUTABLE_NAME) \n $(DASHES) \n "
	@if [ ! -d ./$(BIN_DIR) ]; then mkdir -p ./$(BIN_DIR); fi
	@$(CXX) $(CPPFLAGS) .obj/*.o -o $(BIN_DIR)/$(PSUEODAPPNAME) $(LD_FLAGS) $(RPATH_CONFIG)
	@echo $(DASHES)

# Compile all source files found in SRC_DIR
compile_all_sources:
	@if [ ! -d ./$(OBJ_DIR) ]; then mkdir -p ./$(OBJ_DIR); fi
	@for src in $(SRC_FILES); do \
		obj=$(OBJ_DIR)/$$(basename $${src%.cpp}.o); \
		echo -e "\n $(DASHES) \n  GENERATING OBJECT FILE : $$(basename $$obj) \n $(DASHES) \n "; \
		$(CXX) $(CPPFLAGS) -c $$src -o $$obj; \
	done
 
generate_objects : $(OBJ_FILES)

$(SRC_DIR)/%.o: $(SRC_DIR)/%.cpp
	@echo -e "\n $(DASHES) \n  GENERATING OBJECT FILE : $(notdir $@) \n $(DASHES) \n "
	@if [ ! -d ./$(OBJ_DIR) ]; then mkdir -p ./$(OBJ_DIR); fi
	@$(CXX) $(CPPFLAGS) -c $< -o $(OBJ_DIR)/$(notdir $@)

# Check necessary folders
check_necessary_dirs:
	@for dir in $(CHECK_DIRS); do \
		if [ ! -d $$dir ]; then \
			echo "*********************************"; \
			echo "$$dir directory does not exist.."; \
			echo "aborting.."; \
			echo "*********************************"; \
			exit 1; \
		fi; \
	done

# Check necessary files
check_necessary_files:
	@for file in $(CHECK_FILES); do \
		if [ ! -f $$file ]; then \
			echo "*********************************"; \
			echo "$$file file does not exist.."; \
			echo "aborting.."; \
			echo "*********************************"; \
			exit 1; \
		fi; \
	done

.PHONY: clean
clean : 
	@echo -e "\n $(DASHES) \n  CLEAN WHITE \n $(DASHES) \n "
	-rm -rf .obj/*
	-rm -rf bin/*
	-rm -f Makefile.coming_from_svn
