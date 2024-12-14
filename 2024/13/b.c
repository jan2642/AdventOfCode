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
    uint64_t total = 0;
    
    while (!feof(fp)) {
        int64_t ax, ay, bx, by, x, y;

        fscanf(fp, "Button A: X+%lld, Y+%lld\n", &ax, &ay);
        fscanf(fp, "Button B: X+%lld, Y+%lld\n", &bx, &by);
        fscanf(fp, "Prize: X=%lld, Y=%lld\n", &x, &y);
        fscanf(fp, "\n");

        x += 10000000000000ll;
        y += 10000000000000ll;

        int64_t det = ax * by - bx * ay;
        if (det != 0) {
            int64_t am = (x * by - y * bx);
            int64_t bm = (y * ax - x * ay);

            if (((am % det) == 0) && ((bm % det) == 0))
                total += (3 * am + bm) / det;
        }
    }

    printf("Total: %llu\n", total);
}
