#!/bin/sh

# mainly just in case I want to build this without going through premake or just not depend on it
# probably wont ever both to make this script work on windows though

set -xe

CC="clang"
LIBS="-lGLEW -lEGL -lGL -lGLU -lOpenGL -lglfw -lportaudio -lm -lz -lbz2"
INCLUDES="-Isrc/engine/stb_image -Isrc/engine/resourceLoaders -Isrc/engine -Isrc/game -Isrc/game/gameStates"
CFLAGS="-m64 -g -Wall -Wextra -Wpedantic"
DEFINES="-DUNUSED=__attribute__((unused))"

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
for d in ${DIRS[@]}; do
	for f in src/"$d"/*.c; do
		$CC $CFLAGS $DEFINES $INCLUDES -o $(echo $f | sed -e "s/\.c/\.o/" -e "s/src/obj/") -c $f
		OBJS+="$(echo $f | sed -e "s/\.c/\.o/" -e "s/src/obj/") "
	done
done

# Link
$CC -o build/openGL-test $OBJS $LIBS
