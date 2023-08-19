#!/bin/sh

# disables warning about variables being split intentionally
# shellcheck disable=SC2086

# mainly just in case I want to build this without going through premake or just not depend on it
# probably wont ever both to make this script work on windows though

set -xe

cd "$(dirname $0)"

# takes ~0.54 seconds with gcc, ~0.35 seconds with clang, and ~0.06 seconds with tcc on my computer
# though when compiling with portaudio it takes ~18 seconds with clang 
# (tcc requires -DSTBI_NO_SIMD in DEFINES to compile)
[ "$CC" ] || CC="clang"

LIBS="-lglfw -lm"
INCLUDES="-Isrc/engine/stb_image -Isrc/engine/resourceLoaders -Isrc/engine -Isrc/game -Isrc/game/gameStates -Isrc/external"
CFLAGS="$CFLAGS -Wall -Wextra -Wpedantic"
DEFINES="$DEFINES -DUNUSED=__attribute__((unused))"

[ "$CC" = tcc ] && DEFINES="$DEFINES -DSTBI_NO_SIMD"

# has to be disabled when compiled with clang because it will scream about the macro using a GNU C extension
[ "$DEBUG_EXTRA" ] && [ "$CC" != clang ] && DEFINES="$DEFINES -DDEBUG_EXTRA" DEBUG=on

[ "$DEBUG" ] && CFLAGS="$CFLAGS -g"

OBJS=""

[ "$NO_ZLIB" ] && DEFINES="$DEFINES -DNO_GZIP_SUPPORT" || LIBS="$LIBS -lz"
[ "$NO_BZLIB2" ] && DEFINES="$DEFINES -DNO_BZIP2_SUPPORT" || LIBS="$LIBS -lbz2"

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
	#OBJS="$OBJS $PORTAUDIO_OBJ"
	LIBS="$LIBS -ljack -lasound" # might need to be changed depending on platform, idk
else
	LIBS="$LIBS -lportaudio"
fi

DIRS="\
engine \
engine/resourceLoaders \
external/stb_image \
external/glad \
game \
game/gameStates \
"

# Set up dirs
rm -rf build/ obj/
mkdir -p build/ obj/

for d in $DIRS; do
	mkdir -p obj/"$d"
done

# Compile

# just define DEBUG in any way to make sure it exits on compilation error
if [ -z "$DEBUG" ]; then
for d in $DIRS; do
	for f in src/"$d"/*.c; do
		OBJNAME=$(echo "$f" | sed -e "s/\.c/\.o/" -e "s/src/obj/")
		$CC $CFLAGS $DEFINES $INCLUDES -o $OBJNAME -c "$f" &
		OBJS="$OBJS $OBJNAME "
	done
done
else
for d in $DIRS; do
	for f in src/"$d"/*.c; do
		OBJNAME=$(echo "$f" | sed -e "s/\.c/\.o/" -e "s/src/obj/")
		$CC $CFLAGS $DEFINES $INCLUDES -o $OBJNAME -c $f
		OBJS="$OBJS $OBJNAME "
	done
done
fi

wait

# Link
# PORTAUDIO_OBJ should be empty when it's not being compiled into the project
$CC $CFLAGS -o build/openGL-test $OBJS $PORTAUDIO_OBJ $LIBS
