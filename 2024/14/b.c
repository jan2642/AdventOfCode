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
    int w, h;
    
    /* Hack to make it easier to switch between sample and real input */
    fscanf(fp, "%dx%d\n", &w, &h);
    int64_t robots[1000][4];
    int n = 0;
    while (!feof(fp)) {
        fscanf(fp, "p=%lld,%lld v=%lld,%lld\n", &robots[n][0], &robots[n][1], &robots[n][2], &robots[n][3]);
        n++;
    }

    char *buf = calloc(w * h + 1, 1); /* + 1 for terminating '\0' for strstr() */
    int steps = 0;
    while (1) {
        memset(buf, '.', w * h);

        for (int i = 0; i < n; i++) {
            int px = (((robots[i][0] + steps * robots[i][2]) % w) + w) % w;
            int py = (((robots[i][1] + steps * robots[i][3]) % h) + h) % h;
            buf[(py * w) + px] = '#';
        }

        if (strstr(buf, "##########")) /* Look for 'structure' */
            break;

        steps++;
    }

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++)
            printf("%c", buf[(y * w) + x]);
        printf("\n");
    }
        
    printf("Steps: %d\n", steps);
}
