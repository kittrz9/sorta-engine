CC = gcc
SHELL = /bin/bash
# I kept putting CFLAGS+="-DSTBI_ONLY_PNG" as a command line argument when doing the make command and thought it would add that to the end of the CFLAGS variable but instead just overwrote it I guess and I wasn't getting warnings lmao
# this needs to be changed if I ever need to use stuff that isn't a png lmao
CFLAGS = $(shell pkg-config --cflags glfw3) -Wall -Wpedantic -O3
INCLUDE = -Isrc/engine -Isrc/game -Isrc/game/gameStates
LIBS = $(shell pkg-config --libs glew glfw3) -lm -lrt -lasound -ljack -pthread
NAME = openGL-Test
SOURCES = $(wildcard src/engine/*.c) $(wildcard src/engine/resourceLoaders/*.c) $(wildcard src/game/*.c) $(wildcard src/game/gameStates/*.c)
OBJS = ${subst src/, obj/, ${subst .c,.o,${SOURCES}}}

# making the stb_image.o file seperate so I don't get the dumb overriding recipe warnings that keep making vim switch to the makefile
OBJS := ${filter-out obj/engine/stb_image.o, ${OBJS}}

# creates the executable normally using object files
${NAME}: obj-dir build-dir obj/engine/stb_image.o ${OBJS} 
	${CC} ${CFLAGS} ${INCLUDE} ${LIBS} -o build/${NAME} ${OBJS} obj/engine/stb_image.o libportaudio.a

# creates an executable with debug symbols and stuff
debug: build-dir ${SOURCES}
	$(CC) $(CFLAGS) $(INCLUDE) -pg -g $(SOURCES) $(LDFLAGS) -o build/${NAME}-debug $(LIBS) libportaudio.a

# removes all builds of the game
clean:
	-rm -rf obj
	-rm ./build/${NAME}*

# creates the directory all the builds of the game go
build-dir:
	-mkdir -p build/

# creates the directory for all the object files
obj-dir:
	-mkdir -p obj/game/gameStates obj/engine/resourceLoaders

.PHONY: build-dir obj-dir debug clean

# rule to make object files
# https://stackoverflow.com/questions/14639794/getting-make-to-create-object-files-in-a-specific-directory
${OBJS}: obj/%.o : src/%.c
	${CC} ${CFLAGS} -pg ${INCLUDE} -c src/$*.c -o obj/$*.o

# compiling stb_image.h with -Os because it takes up quite a bit of space and doesn't really get hit with a performance decrease
# I know it doesn't really matter but I don't really want to have the final executable be over 1 MB just because of this
obj/engine/stb_image.o: src/engine/stb_image.c
	${CC} -Os -c src/engine/stb_image.c -o obj/engine/stb_image.o
