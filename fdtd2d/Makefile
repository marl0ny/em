SHELL = /bin/bash

# Clang
COMPILE = /usr/bin/clang++
FLAGS = -O2 -Wall -g -std=c++11
LIBS = -lm -lGL -lGLEW -lglfw

# GCC
# COMPILE = /usr/bin/g++ 
# FLAGS = -Og -Wall -g
# OPENMP = -fopenmp
# INCLUDE = ${OPENMP}
# LIBS = -lm -lGL -lGLEW -lglfw -fPIC ${OPENMP}

TARGETDIR = .
TARGET = ${TARGETDIR}/program
TARGET2 = ${TARGETDIR}/fdtd-2d.js
SOURCES = main.cpp gl_wrappers.cpp
HEADER = shaders.hpp
OBJECTS = main.o gl_wrappers.o
SHADERS = ./shaders/e_field.frag ./shaders/h_field.frag ./shaders/quad.vert \
./shaders/simple_out.frag ./shaders/view.frag ./shaders/wavepacket.frag

all: ${TARGET}

${TARGET}: ${OBJECTS}
	${COMPILE} ${FLAGS} -o $@ $^ ${LIBS}

${OBJECTS}: ${SOURCES} ${HEADER}
	${COMPILE} ${FLAGS} -c ${SOURCES} ${INCLUDE}

${HEADER}: ${SHADERS}
	python3 make_shaders_h.py

${TARGET2}: ${SOURCES}
	python3 -m make_shaders_h --webgl
	EMCC_DEBUG=1
	emcc -o $@ $^ -std=c++11 -O3 -s WASM=1 -s USE_GLFW=3 -s FULL_ES2=1

clean:
	rm -f *.o *~ *.gch ${TARGET} *.wasm ${TARGET2}

