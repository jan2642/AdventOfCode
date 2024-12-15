#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>

static int directions[4][2] = {
    {  0, -1 }, /* ^  up */
    {  1,  0 }, /* >  right */
    {  0,  1 }, /* <  down */
    { -1,  0 }  /* v  left */
};

#define pos(x, y) buf[((y) * w) + (x)]  /* Character at x,y */

int can_move(char *buf, int w, int h, int x, int y, int dir)
{
    int dx = directions[dir][0];
    int dy = directions[dir][1];
    char c = pos(x + dx, y + dy);
    int r = 1;

    if (c == '#')
        return 0;

    if (c == '.')
        return 1;

    if (c == '[' || c == ']') {
        r = can_move(buf, w, h, x + dx, y + dy, dir);
        if (r && (dir == 0 || dir == 2)) { /* Vertical */
            if (c == '[') /* Hit left side, also check right side */
                r = r && can_move(buf, w, h, x + dx + 1, y + dy, dir);
            else          /* Hit right side, also check left side */
                r = r && can_move(buf, w, h, x + dx - 1, y + dy, dir);
        }
    }

    return r;
}

int move(char *buf, int w, int h, int x, int y, int dir)
{
    int dx = directions[dir][0];
    int dy = directions[dir][1];
    char c = pos(x + dx, y + dy);

    if (c == '[' || c == ']') {
        move(buf, w, h, x + dx, y + dy, dir);
        if (dir == 0 || dir == 2) { /* Vertical */
            if (c == '[') /* Hit left side, also move right side */
                move(buf, w, h, x + dx + 1, y + dy, dir);
            else          /* Hit right side, also move left side */
                move(buf, w, h, x + dx - 1, y + dy, dir);
        }
    }

    pos(x + dx, y + dy) = pos(x, y);
    pos(x, y) = '.';

    return 1;
}

int main(int argc, char ** argv)
{
    if (argc != 2) {
        printf("%s <input.txt>\n", argv[0]);
        exit(1);
    }

    FILE *f = fopen(argv[1], "r");
    char buf[1024 * 32];
    char *line = NULL;
    size_t linecap = 0;
    ssize_t linelen;
    int x = 0, y = 0, w = 0, h = 0, px = 0, py = 0;

    /* Read the map */
    while ((linelen = getline(&line, &linecap, f)) > 0) {
        char *c = line;
        if (*c == '\n')
            break;
        while (*c) {
            if (*c == '\n')
                break;
            switch (*c) {
                case '#': pos(x, y) = '#'; pos(x + 1, y) = '#'; break;
                case '@': pos(x, y) = '@'; pos(x + 1, y) = '.'; break;
                case 'O': pos(x, y) = '['; pos(x + 1, y) = ']'; break;
                case '.': pos(x, y) = '.'; pos(x + 1, y) = '.'; break;
            }
            if (*c == '@') { /* Start position */
                px = x;
                py = y;
            }
            x += 2; c++;
        }
        w = x; x = 0; y++;
    }
    h = y;

    printf("%d x %d  start %d,%d\n", w, h, px, py);

    /* Print map */
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++)
            printf("%c", pos(x, y));
        printf("\n");
    }

    /* Read the moves */
    int dir = 0;
    while ((linelen = getline(&line, &linecap, f)) > 0) {
        char *c = line;
        while (*c) {
            if (*c == '\n') break;
            switch(*c) {
                case '^': dir = 0; break;
                case '>': dir = 1; break;
                case 'v': dir = 2; break;
                case '<': dir = 3; break;
            }

            if (can_move(buf, w, h, px, py, dir)) {
                move(buf, w, h, px, py, dir);
                px += directions[dir][0];
                py += directions[dir][1];
            }

            c++;
        }
    }
    fclose(f);

    /* Print map */
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++)
            printf("%c", pos(x, y));
        printf("\n");
    }

    uint64_t total = 0;
    for (int y = 0; y < h; y++)
        for (int x = 0; x < w; x++)
            if (pos(x, y) == '[')
                total += 100 * y + x;

    printf("Total: %llu\n", total);

    return 0;
}
