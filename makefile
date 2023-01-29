NAME = LUA
APP_NAME = Lua
APP_VERSION = 5.0.0.0000
APP_INST_NAME = LUAINST
DESCRIPTION = Lua port by commandblockguy

LUA_CORE = \
	lua/lzio.c \
	lua/lctype.c \
	lua/lopcodes.c \
	lua/lmem.c \
	lua/lundump.c \
	lua/ldump.c \
	lua/lstate.c \
	lua/lgc.c \
	lua/llex.c \
	lua/lcode.c \
	lua/lparser.c \
	lua/ldebug.c \
	lua/lfunc.c \
	lua/lobject.c \
	lua/ltm.c \
	lua/lstring.c \
	lua/ltable.c \
	lua/ldo.c \
	lua/lvm.c \
	lua/lapi.c

LUA_LIBRARIES = \
	lua/lbaselib.c \
	lua/lcorolib.c \
	lua/ldblib.c \
	lua/lmathlib.c \
	lua/lstrlib.c \
	lua/ltablib.c

# 	lua/liolib.c \
	lua/loslib.c \

SRCDIR = src
EXTRA_C_SOURCES = $(LUA_CORE) lua/lauxlib.c $(LUA_LIBRARIES)
EXTRA_HEADERS = lua/*.h

SHARED_FLAGS = -Ilua -Oz \
	-Wfatal-errors \
	-Wextra \
	-Wshadow \
	-Wsign-compare \
	-Wundef \
	-Wwrite-strings \
	-Wredundant-decls \
	-Wdisabled-optimization \
	-Wdouble-promotion \
	-Wmissing-declarations

CFLAGS = $(SHARED_FLAGS) \
	-Wmissing-prototypes \
	-Wnested-externs \
	-Wstrict-prototypes \
	-Wc++-compat \
	-Wold-style-definition
CXXFLAGS = $(SHARED_FLAGS)

# ----------------------------

include app_tools/makefile
