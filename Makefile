CC=g++
CC_FLAGS=-std=c++17 -O2 -ldl -lglfw
LD=ld
BUILD_DIR=./build/
TARGET_FILE=triangle
SRC_DIR=./src/
SRCS=$(shell find $(SRC_DIR) -type f -name "*.cpp")
OBJS=$(patsubst $(SRC_DIR)%.cpp, $(BUILD_DIR)/%.o,$(SRCS))
INCLUDES=\
-I./include/ \
-I./include/imgui/ \
-I./include/KHR/ \
-I./include/opengl/ \
-I./include/game/ \
-I./include/dev/ \

test:
	echo $(OBJS)

# Rule to compile each source file into object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CC) $(CC_FLAGS) $(INCLUDES) -c $< -o $@


# Rule to link object files into executable
$(BUILD_DIR)/$(TARGET_FILE): $(OBJS)
	$(CC) $^ -o $@ $(CC_FLAGS)
	@chmod +x $@

all: $(BUILD_DIR)/$(TARGET_FILE)
	chmod +x $^


# Run target
run: $(BUILD_DIR)/$(TARGET_FILE)
	@./$(BUILD_DIR)/$(TARGET_FILE)


clean:
	rm -rf $(BUILD_DIR)