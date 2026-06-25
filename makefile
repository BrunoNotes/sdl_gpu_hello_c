config ?= debug
# config ?= release

OUTPUT := origami.bin

CC := clang
CFLAGS := -Wall -Wextra
DEBUG_FLAGS := -DDEBUG -g
# DEBUG_FLAGS := -DDEBUG -g -fsanitize=address
RELEASE_FLAGS := -DNDEBUG -O3

IFLAGS := -I./vendor/include
LFLAGS := -L./vendor/lib -lm -lSDL3

ifeq ($(config), debug)
	CFLAGS += $(DEBUG_FLAGS)
else
	CFLAGS += $(RELEASE_FLAGS)
endif

all: build run

run:
	@./bin/$(OUTPUT)

build:
	$(CC) src/main.c $(CFLAGS) $(IFLAGS) $(LFLAGS) -o bin/$(OUTPUT)
