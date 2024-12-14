#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
    if (argc != 2) {
        printf("%s <input.txt>\n", argv[0]);
        exit(1);
    }

    FILE *fp = fopen(argv[1], "r");
    char *line = NULL;
    size_t linecap = 0;
    ssize_t linelen;

    char **lines = calloc(1000, sizeof(char *));
    int total = 0;
    int n = 0;
    while ((linelen = getline(&line, &linecap, fp)) > 0) {
        lines[n++] = strdup(line);
    }
    int w = strlen(lines[0]) - 1;
    int h = n;

    printf("%d X %d\n", w, n);

    char *xmas = "XMAS";
    int len = strlen(xmas);

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            for (int dx = -1; dx <= 1; dx++) {
                for (int dy = -1; dy <= 1; dy++) {
                    if (dx == 0 && dy == 0) continue; /* No movement... */

                    /* Verify bounds */
                    if (((x + (len - 1) * dx) < 0) || ((x + (len - 1) * dx) >= w)) continue;
                    if (((y + (len - 1) * dy) < 0) || ((y + (len - 1) * dy) >= h)) continue;
                    
                    int m = 0;
                    for (int c = 0; c < len; c++)
                        m += (lines[y + c * dy][x + c * dx] == xmas[c]);
                    total += (m == len);
                }
            }
        }
    }

    printf("total: %d\n", total);
    
}
