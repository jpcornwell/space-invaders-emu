
main: main.c
	gcc main.c cpu.c display.c -Wall -Wextra $(shell sdl2-config --cflags) -lSDL2 -o space-invaders
