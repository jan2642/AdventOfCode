#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>

typedef struct queue queue;

struct queue {
    int x, y, steps;
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

static void add_tail(int x, int y, int steps)
{
    queue * q = calloc(1, sizeof(queue));
    q->x = x;
    q->y = y;
    q->steps = steps;
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

#define pos(x, y)  buf[((y) * w) + (x)]  /* Character at x,y */
#define step(x, y) steps[((y) * w) + (x)]  /* Steps to reach x,y  */

static int run_bfs(char * buf, int * steps, int w, int h, int x, int y)
{
    add_tail(x, y, 0); /* Add the start position to the queue */

    queue * q;
    int max_steps = 0;
    while ((q = pop_head())) {
        if (step(q->x, q->y) != -1) { /* Already visited */
            free(q);
            continue;
        }

        step(q->x, q->y) = q->steps;

        if (pos(q->x, q->y) == 'E') {
            max_steps = q->steps;
            free(q);
            break;
        }

        /* Look in every direction */
        for (int i = 0; i < 4; i++) {
            x = q->x + directions[i][0];
            y = q->y + directions[i][1];

            if (x < 0 || y < 0 || x >= w || y >= h) /* Out of bounds */
                continue;

            if ((pos(x, y) == '.' || pos(x, y) == 'E'))
                if (step(x, y) == -1)
                    add_tail(x, y, q->steps + 1);
        }

        free(q);
    }
    
    while ((q = pop_head())) free(q);

    return max_steps;
}


static int run_bfs_2(char * buf, int * steps, int * shortcuts, int w, int h, int sx, int sy, int depth)
{
    add_tail(sx, sy, 0); /* Add the start position to the queue */

    queue * q;
    while ((q = pop_head())) {
        if (pos(q->x, q->y) == '@') { /* Already visited */
            free(q);
            continue;
        }

        if ((pos(q->x, q->y) == '.' || pos(q->x, q->y) == 'E')) /* Hit path, account time saved */
            if (step(q->x, q->y) > step(sx, sy)) /* Only shortcut forward */
                shortcuts[step(q->x, q->y) - step(sx, sy) - q->steps]++;

        pos(q->x, q->y) = '@'; /* Mark visited */

        if (q->steps == depth) { /* Too far */
            free(q);
            continue;
        }

        /* Look in every direction */
        for (int i = 0; i < 4; i++) {
            int x = q->x + directions[i][0];
            int y = q->y + directions[i][1];

            if (x < 0 || y < 0 || x >= w || y >= h) /* Out of bounds */
                continue;
           
            if (pos(x, y) != '@')     
                add_tail(x, y, q->steps + 1);
        }

        free(q);
    }
    
    return 0;
}

int main(int argc, char ** argv)
{
    if (argc != 2) {
        printf("%s <input.txt>\n", argv[0]);
        exit(1);
    }

    FILE *f = fopen(argv[1], "r");
    char buf[1024 * 32];
    int x = 0, y = 0, w = 0, h = 0, sx = 0, sy = 0;

    x = 0; y = 0;
    while (!feof(f)) {
        char c = fgetc(f);
        if (feof(f))
            break;
        if (c == '\n') {
            w = x; x = 0; y++;
            continue;
        }
        pos(x, y) = c;
        if (c == 'S') { sx = x; sy = y; }
        x++;
    }
    h = y;
    fclose(f);

    printf("%d x %d start: %d,%d\n", w, h, sx, sy);

    /* First determine the amount of steps to reach each spot */
    int *steps = calloc(w * h, sizeof(int));
    memset(steps, 0xff, w * h * sizeof(int)); /* Initialize at -1 */
    int max_steps = run_bfs(buf, steps, w, h, sx, sy);

    /* Then search shortcuts starting from every position on the path */
    int *shortcuts = calloc(max_steps, sizeof(int));
    char *buf2 = calloc(w * h, 1);
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            if (pos(x, y) == '.' || pos(x, y) == 'S') { 
                memcpy(buf2, buf, w * h);
                run_bfs_2(buf2, steps, shortcuts, w, h, x, y, 2);
            }
        }
    }

    int total = 0;
    for (int i = 0; i < max_steps; i++) {
        if (i < 100 && shortcuts[i] > 0)
            printf("%3d: %d\n", i, shortcuts[i]);
        if (i >= 100)
            total += shortcuts[i];
    }
    printf("Total: %d\n", total);

}
