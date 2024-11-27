# Compiler settings
CC = gcc
CFLAGS = -Wall -Wextra -O2

# Library paths (adjust if necessary)
INCLUDE_PATHS = -I/opt/homebrew/Cellar/ffmpeg/7.0.2_1/include
LIB_PATHS = -L/opt/homebrew/Cellar/ffmpeg/7.0.2_1/lib

# Libraries to link against
LIBS = -lavformat -lavcodec -lavutil

# Target executable name
TARGET = main

# Default target
all: $(TARGET)

# Linking the executable
$(TARGET): main.o
	$(CC) $(LIB_PATHS) -o $(TARGET) main.o $(LIBS)

# Compiling the object file
main.o: main.c
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) -c main.c

# Clean up compiled files
clean:
	rm -f *.o $(TARGET)

.PHONY: all clean

# CC=gcc
# CFLAGS=-I /opt/homebrew/Cellar/ffmpeg/7.0.2_1/include
# TARGET=main
# SRC=main.c

# all: $(TARGET)

# $(TARGET): $(SRC)
# 	$(CC) -o $@ $< $(CFLAGS) 

# clean:
# 	rm -f $(TARGET)


