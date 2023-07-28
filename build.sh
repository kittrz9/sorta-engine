#!/bin/sh

# mainly just in case I want to build this without going through premake or just not depend on it
# probably wont ever both to make this script work on windows though

set -xe

# takes ~0.6 seconds with gcc, ~0.45 seconds with clang, and ~0.075 seconds with tcc on my computer
# (tcc requires -DSTBI_NO_SIMD in DEFINES to compile, and text is broken when compiling with tcc)
[ "$CC" ] || CC="clang"

LIBS="-lGLEW -lEGL -lGL -lGLU -lOpenGL -lglfw -lportaudio -lm -lz -lbz2"
INCLUDES="-Isrc/engine/stb_image -Isrc/engine/resourceLoaders -Isrc/engine -Isrc/game -Isrc/game/gameStates"
CFLAGS="-g -Wall -Wextra -Wpedantic"
DEFINES="-DUNUSED=__attribute__((unused))"

[ "$CC" == tcc ] && DEFINES="$DEFINES -DSTBI_NO_SIMD"

OBJS=""

DIRS="\
engine \
engine/resourceLoaders \
engine/stb_image \
game \
game/gameStates \
"

# Set up dirs
rm -rf build/ obj/
mkdir -p build/ obj/

for d in ${DIRS[@]}; do
	mkdir obj/"$d"
done

# Compile

# just define DEBUG in any way to make sure it exits on compilation error
if [ -z $DEBUG ]; then
for d in ${DIRS[@]}; do
	for f in src/"$d"/*.c; do
		$CC $CFLAGS $DEFINES $INCLUDES -o $(echo $f | sed -e "s/\.c/\.o/" -e "s/src/obj/") -c $f &
		OBJS+="$(echo $f | sed -e "s/\.c/\.o/" -e "s/src/obj/") "
	done
done
else
for d in ${DIRS[@]}; do
	for f in src/"$d"/*.c; do
		$CC $CFLAGS $DEFINES $INCLUDES -o $(echo $f | sed -e "s/\.c/\.o/" -e "s/src/obj/") -c $f
		OBJS+="$(echo $f | sed -e "s/\.c/\.o/" -e "s/src/obj/") "
	done
done
fi

wait

# Link
$CC $CFLAGS -o build/openGL-test $OBJS $LIBS
