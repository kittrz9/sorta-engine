#!/bin/sh

# disables warning about variables being split intentionally
# shellcheck disable=SC2086

# mainly just in case I want to build this without going through premake or just not depend on it
# probably wont ever both to make this script work on windows though

set -xe

cd "$(dirname $0)"

[ "$CC" ] || CC="clang"

LIBS="-lm"
INCLUDES="-Isrc/engine/stb_image -Isrc/engine/resourceLoaders -Isrc/engine -Isrc/game -Isrc/game/gameStates -Isrc/external"
CFLAGS="$CFLAGS -Os -Wall -Wextra -Wpedantic"
LDFLAGS="$LDFLAGS -Os"
DEFINES="$DEFINES -DUNUSED=__attribute__((unused))"

# tcc requires -DSTBI_NO_SIMD in DEFINES to compile for some reason
[ "$CC" = tcc ] && DEFINES="$DEFINES -DSTBI_NO_SIMD"

# has to be disabled when compiled with clang because it will scream about the macro using a GNU C extension
[ "$DEBUG_EXTRA" ] && [ "$CC" != clang ] && DEFINES="$DEFINES -DDEBUG_EXTRA" DEBUG=on

[ "$DEBUG" ] && CFLAGS="$CFLAGS -g"

OBJS=""

[ "$NO_ZLIB" ] && DEFINES="$DEFINES -DNO_GZIP_SUPPORT" || LIBS="$LIBS -lz"
[ "$NO_BZLIB2" ] && DEFINES="$DEFINES -DNO_BZIP2_SUPPORT" || LIBS="$LIBS -lbz2"

if [ ! "$PORTAUDIO_EXTERNAL" ]; then
	if [ ! -f "src/external/portaudio/configure" ]; then
		printf "\n\nThe PortAudio submodule was not found, yet PORTAUDIO_EXTERNAL was not set.\nIf you have PortAudio installed elsewhere as a library, please set the PORTAUDIO_EXTERNAL environment variable.\nIf you still want to compile it in with the project, please do \`git submodule update --init\` to clone it into the correct place.\n\n"
		exit 1
	fi
	# since src/engine/audio.c will use `#include "portaudio.h"` just doing this in case portaudio isn't installed
	INCLUDES="$INCLUDES -Isrc/external/portaudio/include"
fi

if [ ! "$GLFW_EXTERNAL" ]; then
	if [ ! -f "src/external/glfw/README.md" ]; then
		printf "\n\nThe GLFW submodule was not found, yet GLFW_EXTERNAL was not set.\nIf you have GLFW installed elsewhere as a library, please set the GLFW_EXTERNAL environment variable.\nIf you still want to compile it in with the project, please do \`git submodule update --init\` to clone it into the correct place.\n\n"
		exit 1
	fi
	DIRS="$DIRS external/glfw/src"
	INCLUDES="$INCLUDES -Isrc/external/glfw/include"
	DEFINES="$DEFINES -D_GLFW_X11" # this need to be changed per platform https://www.glfw.org/docs/latest/compile.html#compile_manual
else
	LIBS="$LIBS -lglfw"
fi

# Set up dirs
DIRS="$DIRS \
external/stb_image \
external/glad \
engine \
engine/resourceLoaders \
game \
game/gameStates \
"

rm -rf build/
mkdir -p build/ obj/

for d in $DIRS; do
	mkdir -p obj/"$d"
done

# Compile

# just define DEBUG in any way to make sure it exits on compilation error
for d in $DIRS; do
	for f in src/"$d"/*.c; do
		OBJNAME=$(echo "$f" | sed -e "s/\.c/\.o/" -e "s/src/obj/")
		if [ ! -e "$OBJNAME" ] || [ $(stat "$OBJNAME" -c %Y) -lt $(stat "$f" -c %Y) ]; then
			# should probably find a better way to make it not multithreaded when debugging
			if [ -z "$DEBUG" ]; then
				$CC $CFLAGS $DEFINES $INCLUDES -o $OBJNAME -c "$f" &
			else
				$CC $CFLAGS $DEFINES $INCLUDES -o $OBJNAME -c $f
			fi
		fi
		OBJS="$OBJS $OBJNAME "
	done
done

# build portaudio if not using it externally
if [ ! "$PORTAUDIO_EXTERNAL" ]; then
	if [ ! -f "src/external/portaudio/configure" ]; then
		printf "\n\nThe PortAudio submodule was not found, yet PORTAUDIO_EXTERNAL was not set.\nIf you have PortAudio installed elsewhere as a library, please set the PORTAUDIO_EXTERNAL environment variable.\nIf you still want to compile it in with the project, please do \`git submodule update --init\` to clone it into the correct place.\n\n"
		exit 1
	fi
	PORTAUDIO_OBJ="src/external/portaudio/lib/.libs/libportaudio.a"
	if [ ! -f "$PORTAUDIO_OBJ" ]; then
		cd src/external/portaudio
		./configure
		make -j12
		cd -
	fi
	OBJS="$OBJS $PORTAUDIO_OBJ"
	LIBS="$LIBS -ljack -lasound" # might need to be changed depending on platform, idk
else
	LIBS="$LIBS -lportaudio"
fi

wait

# Link
$CC $LDFLAGS -o build/openGL-test $OBJS $LIBS
