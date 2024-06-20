CC=g++
CC_FLAGS=-std=c++17 -O2 -ldl -lglfw

TARGET_FILE=triangle
CSRC=$(shell find . -type f -name "*.cpp")
INCLUDES=\
-I./include/ \
-I./include/imgui/ \
-I./include/KHR/ \



all:
	$(CC) $(INCLUDES) $(CSRC) -o $(TARGET_FILE) $(CC_FLAGS) 
	chmod +x $(TARGET_FILE)


run:
	./triangle