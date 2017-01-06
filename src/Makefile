CC = g++
CFLAGS = -Wall -std=c++11 -g
TARGET = main

INC_PATH = -I ../shared/ \
	-I ../shared/include/ \
	-I ../shared/gl3w/ \
	-I ../shared/lodepng/ \
	-I /usr/local/boost_1_61_0/ \

LIB_PATH = -L ../lib/ \

LDLIBS = -llodepng \
	-pthread \
	-lglfw3 \
	-lGL \
	-lboost_program_options \
	-lpthread \

CFLAGS += $(INC_PATH)

DEPS = raytracer.o \
	image.o \
	surface.o \
	light.o \
	material.o \
	triangle.o \
	objobject.o \
	transform.o \
	bvh.o \

all: $(TARGET)

$(TARGET): clean $(DEPS) $(TARGET).cpp
	$(CC) $(CFLAGS) $(LIB_PATH) $(DEPS) $(TARGET).cpp -o raytrace $(LDLIBS) -Wl,--no-as-needed

raytracer.o: raytracer.cpp
	$(CC) $(CFLAGS) -c raytracer.cpp -o raytracer.o

image.o: image.cpp
	$(CC) $(CFLAGS) -c image.cpp -o image.o

surface.o: surface.cpp
	$(CC) $(CFLAGS) -c surface.cpp -o surface.o

light.o: light.cpp
	$(CC) $(CFLAGS) -c light.cpp -o light.o

material.o: material.cpp
	$(CC) $(CFLAGS) -c material.cpp -o material.o

objobject.o: objobject.cpp
	$(CC) $(CFLAGS) -c objobject.cpp -o objobject.o

triangle.o: triangle.cpp
	$(CC) $(CFLAGS) -c triangle.cpp -o triangle.o

transform.o: transform.cpp
	$(CC) $(CFLAGS) -c transform.cpp -o transform.o

bvh.o: bvh.cpp
	$(CC) $(CFLAGS) -c bvh.cpp -o bvh.o

clean:
	rm -f *.o raytrace
