#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>

typedef struct {
    int x;
    int y;
} pos_t;

int main(int argc, char ** argv)
{
    if (argc != 2) {
        printf("%s <input.txt>\n", argv[0]);
        exit(1);
    }

    FILE *fp = fopen(argv[1], "r");
	char buf[1024 * 32];
	int x = 0, y = 0, w = 0, h = 0;

    char *freqs = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

    pos_t antenna[62][10];
    int n_ant[62] = { 0 };

	while (!feof(fp)) {
		char c = fgetc(fp);
        if (feof(fp))
            break;

		if (c == '\n') {
			if (w == 0) w = x;
			x = 0; y++;
			continue;
		}
		buf[(y * w) + x] = c;

        if (c != '.') {
            char *p = index(freqs, c);
            if (!p)
                abort();
            int f = p - freqs;
            //printf("freq: %c\n", freqs[f]);

            antenna[f][n_ant[f]].x = x;
            antenna[f][n_ant[f]].y = y;
            n_ant[f]++;
        }

		x++;
	}
	h = y;
	fclose(fp);

	printf("%d x %d\n", w, h);

#if 0
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++)
            printf("%c", buf[(y * w) + x]);
        printf("\n");
    }
    printf("\n");
#endif

    int nx, ny;
    int unique = 0;

    for (int f = 0; f < 62; f++) {
        if (n_ant[f]) {
            for (int p = 0; p < n_ant[f] - 1; p++) {
                for (int q = p + 1; q < n_ant[f]; q++) {
                    int dx = antenna[f][q].x - antenna[f][p].x;
                    int dy = antenna[f][q].y - antenna[f][p].y;

                    nx = antenna[f][p].x - dx;
                    ny = antenna[f][p].y - dy;

                    if (nx >= 0 && nx < w && ny >= 0 && ny < h && buf[(ny * w) + nx] != '#') {
                        buf[(ny * w) + nx] = '#';
                        unique++;
                    }
                    
                    nx = antenna[f][q].x + dx;
                    ny = antenna[f][q].y + dy;

                    if (nx >= 0 && nx < w && ny >= 0 && ny < h && buf[(ny * w) + nx] != '#') {
                        buf[(ny * w) + nx] = '#';
                        unique++;
                    }

                    //printf("%d - %d   -> %d %d\n", p, q, nx, ny);
                }
            }
        }
    }

#if 0
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++)
            printf("%c", buf[(y * w) + x]);
        printf("\n");
    }
#endif

#if 0
    for (int f = 0; f < 62; f++)
        for (int n = 0; n < n_ant[f]; n++)
            printf("%c: %d %d\n", freqs[f], antenna[f][n].x, antenna[f][n].y);
#endif
    printf("Unique: %d\n", unique);

	return 0;
}
