CFLAGS = $(INCLUDES)
CXXFLAGS =-O0 -g -Wall -Wno-sign-compare -Wno-narrowing -std=c++11 $(DEFINES) $(INCLUDES)
CXX = LANG=C g++

MKDIR=mkdir -p

src_to_obj = $(patsubst $(1)/%.c,$(BIN_DIR)/$(1)/%.o,$(wildcard $(1)/*.c)) $(patsubst $(1)/%.cpp,$(BIN_DIR)/$(1)/%.o,$(wildcard $(1)/*.cpp))
src_to_deps = $(patsubst $(1)/%.c,$(DEP_DIR)/$(1)/%.d,$(wildcard $(1)/*.c)) $(patsubst $(1)/%.cpp,$(DEP_DIR)/$(1)/%.d,$(wildcard $(1)/*.cpp))

OBJS =  $(foreach dir,$(SRC_DIRS),$(call src_to_obj,$(dir)))
DEPS = $(foreach dir,$(SRC_DIRS),$(call src_to_deps,$(dir)))
HEADERS = $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.h))

INCLUDES =-I.

# LIBS = -L$(TILEDPP_LIB_DEBUG) -lTiledPP -ltinyxml2 -lticppd -lz -lSDL_gfx -lSDL -lSDL_image \
# 		-lboost_filesystem-mgw47-mt-sd-1_52 -lboost_thread-mgw47-mt-sd-1_52 \
#		-lboost_system-mgw47-mt-sd-1_52 -lboost_chrono-mgw47-mt-sd-1_52

### Parameters
TARGET = $(BIN_DIR)/tiledpp.a
TARGET_BIN_DIR = lib
TARGET_INCLUDE_DIR = include

DEFINES = 
#-D__GXX_EXPERIMENTAL_CXX0X__ -DBOOST_THREAD_USE_LIB -DRES_DIR=\"$(RES_DIR)\" -DMAPS_DIR=\"$(MAPS_DIR)\"

# Folders
SRC_DIRS = src
BIN_DIR = bin
DEP_DIR = dep

RES_DIR = res
MAPS_DIR = maps

.PHONY: all clean clean-bin clean-dep rebuild run package-all src-archive dist

all: $(TARGET)

$(TARGET): $(OBJS)
	@$(MKDIR) $(@D)
	$(AR) rcs $@ $^

clean: clean-bin clean-dep

clean-bin:
	rm -rf $(BIN_DIR) $(TARGET)

clean-dep:
	rm -rf $(DEP_DIR)

rebuild: clean all

run: $(TARGET)
	@echo -----
	@./$<

package-all: src-archive dist

src-archive: $(SRC_DIRS)/* Makefile
	7z a -r src.zip $^

dist: $(TARGET)
	$(MKDIR) archive_struct
	$(MKDIR) archive_struct/include
	cp -t archive_struct/include $(HEADERS)
	$(MKDIR) archive_struct/lib
	cp -t archive_struct/lib $^
	7z a -r dist.zip ./archive_struct/*

### File generation rules
# Object files
$(BIN_DIR)/%.o: %.c
	@$(MKDIR) $(@D)
	$(C) $(CFLAGS) -c -o $@ $<

$(BIN_DIR)/%.o: %.cpp
	@$(MKDIR) $(@D)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Dependency files
$(DEP_DIR)/%.d: %.c
	@$(MKDIR) $(@D)
	$(C) $(CFLAGS) -MM -MT"$(patsubst %.c,$(BIN_DIR)/%.o,$<)" -MT"$@" -MF"$@" $<

$(DEP_DIR)/%.d: %.cpp
	@$(MKDIR) $(@D)
	$(CXX) $(CXXFLAGS) -MM -MT"$(patsubst %.cpp,$(BIN_DIR)/%.o,$<)" -MT"$@" -MF"$@" $<

# DEPENDENCIES
ifneq ($(MAKECMDGOALS),clean)
  -include $(DEPS)
endif