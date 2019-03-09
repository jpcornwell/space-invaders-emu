
main: main.c
	gcc main.c cpu.c display.c audio.c -Wall -Wextra $(shell sdl2-config --cflags) -lSDL2 -lSDL2_mixer -o space-invaders
