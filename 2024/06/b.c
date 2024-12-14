#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>

typedef struct {
    int dx;
    int dy;
    char c;
    char cross;
} dir_t;

dir_t directions[4] = { 
    {  0, -1, '|', '-' }, /* up */
    {  1,  0, '-', '|' }, /* right */
    {  0,  1, '|', '-' }, /* down */
    { -1,  0, '-', '|'}, /* left */
};

int main(int argc, char ** argv)
{
    if (argc != 2) {
        printf("%s <input.txt>\n", argv[0]);
        exit(1);
    }

    FILE *fp = fopen(argv[1], "r");
	char buf_org[1024 * 32];
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
		buf_org[(y * w) + x] = c;

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

    int loops = 0;

    for (int yy = 0; yy < h; yy++) {
        for (int xx = 0; xx < w; xx++) {

            /* Reset */
            memcpy(buf, buf_org, sizeof(buf));
            x = sx;
            y = sy;
            dir = 0;

            if (buf[(yy * w) + xx] != '.')
                continue;

            /* Put an obstacle at xx,yy */
            buf[(yy * w) + xx] = 'O';
            int loop = 0;
            int steps = 0;

            while (1) {
                /* Mark current location */
                char c = buf[(y * w) + x];

                if (steps > w * h) {
                    /* More steps than spots on the map --> Loop detected ! */
                    loop = 1;
                    break;
                }

                if (c == '.' || c == '^')
                    buf[(y * w) + x] = directions[dir].c;
                else if (c == directions[dir].cross)
                    buf[(y * w) + x] = '+';

                /* Next position */
                int nx = x + directions[dir].dx;
                int ny = y + directions[dir].dy;

                /* Check border */
                if (nx < 0 || nx >=w || ny < 0 || ny >= h) /* Out of the map */
                    break;

                /* Collision ? */
                if ((buf[(ny * w) + nx] == '#') ||
                    (buf[(ny * w) + nx] == 'O'))
                    dir = (dir + 1) % 4; /* Turn right */
                else {
                    x = nx;
                    y = ny;
                    steps++; 
                }
            }

#if 0
            for (y = 0; y < h; y++) {
                for (x = 0; x < w; x++)
                    printf("%c", buf[(y * w) + x]);
                printf("\n");
            }
            printf("%d %d --> %d\n\n", xx, yy, loop);
#endif

            loops += loop;
        }
    }

    printf("Loops: %d\n", loops);

	return 0;
}
