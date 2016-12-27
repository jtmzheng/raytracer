CC = g++
CFLAGS = -Wall -std=c++11 -g
LINKS = -pthread
TARGET = main

INC_PATH = -I ../shared/ \
	-I ../shared/include/ \
	-I ../shared/gl3w/ \
	-I ../shared/lodepng/ \

LIB_PATH = -L ../lib/

LDLIBS = -llodepng \
	-lpthread \
	-lglfw3 \
	-lGL \

CFLAGS += $(INC_PATH)

DEPS = raytracer.o \
	image.o \
	surface.o \

all: $(TARGET)

$(TARGET): clean $(DEPS) $(TARGET).cpp
	$(CC) $(CFLAGS) $(LIB_PATH) $(DEPS) $(TARGET).cpp -o raytrace $(LDLIBS)

raytracer.o: raytracer.cpp
	$(CC) $(CFLAGS) -c raytracer.cpp -o raytracer.o

image.o: image.cpp
	$(CC) $(CFLAGS) -c image.cpp -o image.o

surface.o: surface.cpp
	$(CC) $(CFLAGS) -c surface.cpp -o surface.o

clean:
	rm -f *.o raytrace
