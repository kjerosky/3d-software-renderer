EXECUTABLE = triangle-rasterizer-test
CC = g++
FLAGS = --std=c++17 -Wall -g

INCLUDE_PATHS = -I /opt/homebrew/include
LIBRARY_PATHS = -L /opt/homebrew/lib
LIBRARIES = -lSDL3

SOURCE_FILES = \
	main.cpp \
	TriangleRasterizer.cpp

$(EXECUTABLE):
	$(CC) $(FLAGS) -o $(EXECUTABLE) $(SOURCE_FILES) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(LIBRARIES)

clean:
	rm -rf $(EXECUTABLE) *.dSYM
