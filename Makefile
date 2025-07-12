EXECUTABLE = 3d-software-renderer
CC = g++
FLAGS = --std=c++17 -Wall

INCLUDE_PATHS = -I /opt/homebrew/include
LIBRARY_PATHS = -L /opt/homebrew/lib
LIBRARIES = -lSDL3 -lSDL3_image

SOURCE_FILES = \
	main.cpp \
	TriangleRasterizer.cpp \
	Object.cpp \
	Primitives.cpp

$(EXECUTABLE):
	$(CC) $(FLAGS) -o $(EXECUTABLE) $(SOURCE_FILES) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(LIBRARIES)

clean:
	rm -rf $(EXECUTABLE) *.dSYM
