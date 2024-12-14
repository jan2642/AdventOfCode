#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>

typedef struct queue queue;

struct queue {
    int x, y;
    queue * next;
};

static queue * qhead, * qtail;

static queue * pop_head()
{
    queue * res = qhead;
    if (qhead) qhead = qhead->next;
    if (res == qtail) qtail = NULL;
    return res;
}

static void add_tail(int x, int y)
{
    queue * q = calloc(1, sizeof(queue));
    q->x = x;
    q->y = y;
    if (qtail)
        qtail->next = q;
    else
        qhead = q;
    qtail = q;
}

static int directions[4][2] = {
    {  0, -1 }, /* up */
    {  1,  0 }, /* right */
    {  0,  1 }, /* down */
    { -1,  0 } /* left */
};

#define pos(x, y) buf[((y) * w) + (x)]  /* Character at x,y with marked flag */
#define val(x, y) (pos(x, y) & 0x7F)    /* Character at x,y with masked flag */

static int run_bfs(char * buf, int w, int h, int x, int y)
{
    char c = pos(x, y);

    /* Add the start position to the queue */
    add_tail(x, y);

    queue * q;

    int area = 0;
    int perimeter = 0;

    while ((q = pop_head())) {
        if ((pos(q->x, q->y) & 0x80) == 0x80) { /* Already marked */
            free(q);
            continue;
        }

        pos(q->x, q->y) |= 0x80; /* Mark visited */
        area++;

        /* Look in every direction */
        int neighbours = 0;    
        for (int i = 0; i < 4; i++) {
            x = q->x + directions[i][0];
            y = q->y + directions[i][1];

            if (pos(x, y) == c)     /* Unmarked spot with the same character, go there */
                add_tail(x, y);

            if (val(x, y) == c)     /* Count neighbours with the same character */
                neighbours++;
        }
        perimeter += 4 - neighbours;    /* Every side without a neighbour needs a fence */
        free(q);
    }

    printf(" %c: %d x %d = %d\n", c, area, perimeter, area * perimeter);

    return area * perimeter;
}

int main(int argc, char ** argv)
{
    if (argc != 2) {
        printf("%s <input.txt>\n", argv[0]);
        exit(1);
    }

    FILE *f = fopen(argv[1], "r");
    char buf[1024 * 32];
    int x = 0, y = 0, w = 0, h = 0;

    /* First figure out the width to get a border around the field for avoiding out-of-bound issues */
    while (!feof(f)) {
        char c = fgetc(f);
        if (c == '\n')
            break;
        w++;
    }
    w += 2; /* Border */

    memset(buf, '.', sizeof(buf));
    rewind(f);
    x = 1; y = 1;
    while (!feof(f)) {
        char c = fgetc(f);
        if (feof(f))
            break;
        if (c == '\n') {
            x = 1; y++;
            continue;
        }
        buf[(y * w) + x] = c;
        x++;
    }
    h = y;
    h += 1; /* Border */
    fclose(f);

    printf("%d x %d\n", w, h);

    int total = 0;
    for (int y = 1; y < h - 1; y++) {
        for (int x = 1; x < w - 1; x++) {
            char c = pos(x, y);

            if (((c & 0x80) == 0) && c != '.') {
                /* New region found */
                int price = run_bfs(buf, w, h, x, y);
                printf("Region at %d,%d (%c) --> %d\n", x, y, c, price);
                total += price;
            }
        }
    }

    printf("Total: %d\n", total);
    return 0;
}
