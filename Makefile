CC = gcc
SHELL = /bin/bash
CFLAGS = $(shell pkg-config --cflags glfw3) -Wall -Wpedantic -g
INCLUDE = -Isrc/engine -Isrc/game -Isrc/game/gameStates
LIBS = $(shell pkg-config --libs glew glfw3) -lm
NAME = openGL-Test
SOURCES = $(wildcard src/engine/*.c) $(wildcard src/game/*.c) $(wildcard src/game/gameStates/*.c)
OBJS = ${subst src/, obj/, ${subst .c,.o,${SOURCES}}}

# creates the executable normally using object files
${NAME}: obj-dir build-dir ${OBJS}
	${CC} ${CFLAGS} ${INCLUDE} ${LIBS} -o build/${NAME} ${OBJS}

# creates an executable with debug symbols and stuff
debug: build ${SOURCES}
	$(CC) $(CFLAGS) $(INCLUDE) -g $(SOURCES) $(LDFLAGS) -o build/${NAME}-debug $(LIBS)

# removes all builds of the game
clean:
	-rm -rf obj
	-rm ./build/${NAME}*

# creates the directory all the builds of the game go
build-dir:
	-mkdir -p build/

# creates the directory for all the object files
obj-dir:
	-mkdir -p obj/game/gameStates obj/engine

# rule to make object files
# https://stackoverflow.com/questions/14639794/getting-make-to-create-object-files-in-a-specific-directory
${OBJS}: obj/%.o : src/%.c
	${CC} ${CFLAGS} ${INCLUDE} -c src/$*.c -o obj/$*.o
