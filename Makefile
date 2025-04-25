CC      := gcc
CFLAGS  := -Iinclude                                                     
SDL_CFLAGS := $(shell pkg-config --cflags SDL2 SDL2_ttf)

LDFLAGS  := \
    -lmingw32 \
    -lSDL2main \
    -lSDL2 \
    -lSDL2_ttf \
    -lmosquitto                       

SRC     := main.c
OBJ     := $(SRC:.c=.o)
TARGET  := myapp.exe

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $^ -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) $(SDL_CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

