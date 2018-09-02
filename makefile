
main: main.c
	gcc main.c cpu.c display.c -Wall -Wextra -lSDL2 -o space-invaders
