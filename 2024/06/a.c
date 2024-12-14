#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>

typedef struct {
    int dx;
    int dy;
} dir_t;

dir_t directions[4] = { 
    { 0, -1 }, /* up */
    { 1, 0 },  /* right */
    { 0, 1 },  /* down */
    { -1, 0 }, /* left */
};

int main(int argc, char ** argv)
{
    if (argc != 2) {
        printf("%s <input.txt>\n", argv[0]);
        exit(1);
    }

    FILE *fp = fopen(argv[1], "r");
	char buf[1024 * 32];
	int x = 0, y = 0, w = 0, h = 0;
    int sx = 0, sy = 0, dir = 0;

	while (!feof(fp)) {
		char c = fgetc(fp);
		if (c == '\n') {
			if (w == 0) w = x;
			x = 0; y++;
			continue;
		}
		buf[(y * w) + x] = c;

        if (c == '^') {
            sx = x;
            sy = y;
        }
        
		x++;
	}
	h = y;
	fclose(fp);

	printf("%d x %d\n", w, h);
	printf("start: %d,%d %d\n", sx, sy, dir);

    x = sx;
    y = sy;
    int changed = 0;

    while (1) {
        /* Mark current location */
        if (buf[(y * w) + x] != 'X') {
            buf[(y * w) + x] = 'X';
            changed++;
        }

        /* Check border */
        int nx = x + directions[dir].dx;
        int ny = y + directions[dir].dy;

        if (nx < 0 || nx >=w || ny < 0 || ny >= h) /* Out of the map */
            break;

        /* Collision ? */
        if (buf[(ny * w) + nx] == '#')
            dir = (dir + 1) % 4; /* Turn right */
        else {
            x = nx;
            y = ny; 
        }
    }

    printf("Changed: %d\n", changed);

	return 0;
}
