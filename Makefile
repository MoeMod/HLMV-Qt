# simple Makefile for Half-Life Model Viewer

CC  ?= gcc
CXX ?= g++

PREFIX ?= /usr/local
CFLAGS += -O3 -pipe -DNDEBUG
CXXFLAGS =

BUNDLED_MXTK ?= 1
STATIC_MXTK ?= 1

INCLUDE = -Isrc
#LDFLAGS += -s

SYS = $(shell $(CXX) -dumpmachine)
ifneq (, $(findstring mingw, $(SYS)))
APP_NAME = hlmv.exe
PLATFORM = win32
STATIC_LIB_EXT = lib
SHARED_LIB_EXT = dll
else
APP_NAME = hlmv
PLATFORM = qt
STATIC_LIB_EXT = a
SHARED_LIB_EXT = so
endif

BIN_DIR = bin
BIN = $(BIN_DIR)/$(APP_NAME)

ifeq ($(BUNDLED_MXTK), 1)
INCLUDE += -Imxtk/include
ifeq ($(STATIC_MXTK), 1)
MXTK_LIB = mxtk/lib/libmxtk-$(PLATFORM).$(STATIC_LIB_EXT)
else
MXTK_LIB = mxtk/lib/libmxtk-$(PLATFORM).$(SHARED_LIB_EXT)
endif
else
LIBS += $(shell pkg-config --libs mxtk)
endif

LIBS += $(MXTK_LIB)

ifneq (, $(findstring mingw, $(SYS)))
LIBS += -lcomctl32 -lcomdlg32 -lgdi32 -lopengl32 -lwinmm -static-libgcc -static-libstdc++
else
CFLAGS += -fPIC
LIBS += $(shell pkg-config --libs Qt5OpenGL gl)
INCLUDE += $(shell pkg-config --cflags Qt5OpenGL gl)
endif

SRC = $(wildcard src/*.cpp)
C_SRC = src/mathlib.c

OBJS = $(SRC:%.cpp=%.o)
C_OBJS = $(C_SRC:%.c=%.o)

all: $(BIN)

$(MXTK_LIB):
	$(MAKE) -C mxtk/ CC="$(CC)" CXX="$(CXX)" CFLAGS="$(CFLAGS)"

$(BIN): $(MXTK_LIB) $(OBJS) $(C_OBJS)
	mkdir -p $(BIN_DIR)
	$(CXX) $(LDFLAGS) -o $(BIN) $(OBJS) $(C_OBJS) $(LIBS)

.c.o:
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

.cpp.o:
	$(CXX) $(CFLAGS) $(CXXFLAGS) $(INCLUDE) -c $< -o $@

.PHONY: all clean

# clean

clean:
	$(RM) $(OBJS) $(C_OBJS)
