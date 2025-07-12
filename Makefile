TARGET = $(EXEC_DIR)/3d-software-renderer
CC := g++
CC_FLAGS := --std=c++17 -Wall -MMD -MP

SRC_DIR := src
BUILD_DIR := build
OBJ_DIR := $(BUILD_DIR)/objs
EXEC_DIR := $(BUILD_DIR)/executable

SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))

DEPS := $(OBJS:.o=.d)

INCLUDE_DIRS := -I /opt/homebrew/include
LIBRARY_DIRS := -L /opt/homebrew/lib
LIBRARIES := -lSDL3 -lSDL3_image

all: $(TARGET)

$(TARGET): $(OBJS) | $(EXEC_DIR)
	$(CC) -o $@ $(OBJS) $(CC_FLAGS) $(LIBRARY_DIRS) $(LIBRARIES)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CC) -o $@ -c $< $(CC_FLAGS) $(INCLUDE_DIRS)

$(EXEC_DIR): $(BUILD_DIR)
	mkdir -p $(EXEC_DIR)

$(OBJ_DIR): $(BUILD_DIR)
	mkdir -p $(OBJ_DIR)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)

-include $(DEPS)
