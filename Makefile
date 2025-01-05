CC := gcc
CFLAGS := -g -Wall -Wextra -std=c11 -Wno-format -Wno-implicit-fallthrough

SOURCES := $(wildcard src/*.c)
OBJECTS := $(patsubst src/%.c, obj/%.o, $(SOURCES))


.PHONY: clean setup

all: setup simplelang

simplelang: $(OBJECTS)
	$(CC) $^ -o $@


obj/%.o: src/%.c include/%.h
	$(CC) $(CFLAGS) -c $< -o $@

obj/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

setup:
	@mkdir -p obj

clean:
	@rm -rf obj simplelang
