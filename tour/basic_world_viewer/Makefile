
INCLUDE = -I include
BUILD = build/Imageloader.o build/Main.o

INCLUDEFILES = include/Imageloader.h include/model.h include/renderScene.h

CC = g++ -W -O2 -g

UNAME := $(shell uname)
ifeq ($(UNAME), Darwin)
	GLFLAGS = -framework GLUT -framework OpenGL
else
	GLFLAGS = -lglut -lGLU -lGL -ljpeg
endif

default: bin/world_viewer

run: bin/world_viewer
	./bin/world_viewer
	
execute: bin/world_viewer
	./bin/world_viewer

valgrind: bin/world_viewer
	valgrind --tool=memcheck --leak-check=yes --show-reachable=yes ./bin/world_viewer

bin/world_viewer: $(BUILD)
	$(CC) -o bin/world_viewer $(BUILD) $(GLFLAGS)
	
build/Imageloader.o: src/Imageloader.cpp include/Imageloader.h
	$(CC) $(INCLUDE) -c src/Imageloader.cpp -o build/Imageloader.o

build/Main.o: src/Main.cpp include/model.h include/Imageloader.h include/renderScene.h
	$(CC) $(INCLUDE) -c src/Main.cpp -o build/Main.o $(GLFLAGS)

clean:
	rm -f bin/*
	rm -f build/*.o
