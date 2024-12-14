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
    int q[4] = { 0 };
    while (!feof(fp)) {
        int64_t px, py, vx, vy;
        fscanf(fp, "p=%lld,%lld v=%lld,%lld\n", &px, &py, &vx, &vy);

        px = (((px + 100 * vx) % w) + w) % w;
        py = (((py + 100 * vy) % h) + h) % h;

        if ((px < (w / 2)) && (py < (h / 2))) q[0]++;
        if ((px > (w / 2)) && (py < (h / 2))) q[1]++;
        if ((px < (w / 2)) && (py > (h / 2))) q[2]++;
        if ((px > (w / 2)) && (py > (h / 2))) q[3]++;
    }

    uint64_t total = q[0] * q[1] * q[2] * q[3];
    printf("Total: %llu\n", total);
}
