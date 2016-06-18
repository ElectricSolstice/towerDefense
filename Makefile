#!/bin/bash
OS := linux
BUILD := debug

#WIN_64_CXX :=x86_64-w64-mingw32-g++-4.8.3
#ifeq ($(CXX), $(WIN_64_CXX))
#	PATH:=/opt/cross_toolchain_64/bin:$(PATH)
#	WINDOWS=true
#endif

GXX_PROFILING_FLAGS := -pg -fprofile-arcs -ftest-coverage
DEBUG_EXE_GXXFLAGS := -Wall -Wshadow -pedantic -g --std=c++11 -fno-inline
DEBUG_OBJ_GXXFLAGS := -c $(DEBUG_EXE_GXXFLAGS)
OPTIMIZE_EXE_GXXFLAGS := --std=c++11 -O3 -march=native
OPTIMIZE_OBJ_GXXFLAGS := -c $(OPTIMIZE_EXE_GXXFLAGS)
ifeq ($(BUILD), debug)
	EXECUTABLE_CXXFLAGS := $(DEBUG_EXE_GXXFLAGS)
	OBJ_CXXFLAGS := $(DEBUG_OBJ_GXXFLAGS)
endif
ifeq ($(BUILD), release)
	EXECUTABLE_CXXFLAGS := $(OPTIMIZE_EXE_GXXFLAGS)
	OBJ_CXXFLAGS := $(OPTIMIZE_OBJ_GXXFLAGS)
endif

ifeq ($(OS), linux)
	EXECUTABLE_NAME := tower
else
	EXECUTABLE_NAME := tower.exe
endif
PKG_CONFIG_FLAGS = `pkg-config --cflags --libs allegro_ttf-5 allegro_primitives-5 allegro_image-5 allegro_font-5 allegro_color-5 allegro_audio-5 allegro-5 allegro_tiled-5`

SRCS:= $(wildcard *.cpp)
SRCS := $(filter-out main.cpp, $(SRCS))
OBJ_DIR := obj
$(shell mkdir -p $(OBJ_DIR) >/dev/null)
DEP_DIR := dep
$(shell mkdir -p $(DEP_DIR) >/dev/null)
OBJS := $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SRCS))
DEPS := $(patsubst %.cpp,$(DEP_DIR)/%.d,$(SRCS))

all: $(OBJS) $(OBJ_DIR)/main.o
	$(CXX) $(EXECUTABLE_CXXFLAGS) $(PKG_CONFIG_FLAGS) -o $(EXECUTABLE_NAME) $(OBJS) $(OBJ_DIR)/main.o

$(OBJ_DIR)/%.o: %.cpp
	$(CXX) $(OBJ_CXXFLAGS) $(PKG_CONFIG_FLAGS) $< -o $@

$(DEP_DIR)/%.d: %.cpp
	./depend.sh `dirname $*.cpp` $(CXXFLAGS) $*.cpp > $@

include $(DEPS)

.PHONY: clean, test
clean:
	$(RM) $(OBJS) $(TESTS) $(DEPS) $(EXECUTABLE_NAME)

TEST_DIR := test
TEST_SRCS := $(wildcard test/*.cpp)
TESTS := $(basename $(TEST_SRCS))
test: $(OBJS) $(TESTS)
	for test in $(TESTS) ; do \
		$$test ; \
	done
$(TESTS):  % : %.cpp  $(OBJS)
	$(CXX) $(EXECUTABLE_CXXFLAGS) $(PKG_CONFIG_FLAGS) $< -o $@ $(OBJS)
$(TEST_DIR)/%.o: %.cpp
	$(CXX) $(OBJ_CXXFLAGS) $(PKG_CONFIG_FLAGS) $< -o $@
