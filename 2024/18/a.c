#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

typedef struct queue queue;

struct queue {
    int dir;
    int x, y;
    int steps;
    queue * next;
};

queue * qhead, * qtail;

queue * pop_head()
{
    queue * res = qhead;
    if (qhead) qhead = qhead->next;
    if (res == qtail) qtail = NULL;
    return res;
}

void add_tail(int dir, int x, int y, uint32_t steps)
{
    queue * q = calloc(1, sizeof(queue));
    q->dir = dir;
    q->x = x;
    q->y = y;
    q->steps = steps;
    if (qtail)
        qtail->next = q;
    else
        qhead = q;
    qtail = q;
}

static int back[] = { 2, 3, 0, 1, -1 };
static int directions[4][2] = { { 0, -1 }, /* up */ { 1, 0 }, /* right */ { 0, 1 }, /* down */ { -1, 0 } /* left */ };

#define pos(x, y) buf[((y) * w) + (x)]   /* Character at x,y */

uint32_t run_bfs(char * buf, int w, int h, int x, int y)
{
    /* Add the start position to the queue */
    add_tail(4, x, y, 0);

    queue * q;

    uint32_t steps = 0;

    while ((q = pop_head())) {
        if (pos(q->x, q->y) == 'O') { /* Been there, done that */
            free(q);
            continue;
        }

        pos(q->x, q->y) = 'O'; /* Mark visited */

        if ((q->x == (w - 1)) && (q->y == (h - 1))) { /* Found the exit */
            steps = q->steps;
            free(q);
            break;
        }

        for (int i = 0; i < 4; i++) { /* Look in every direction */
            if (q->dir == back[i])
                continue; /* Don't go where we came from */

            x = q->x + directions[i][0];
            y = q->y + directions[i][1];

            if (x < 0 || x >= w || y < 0 || y >= h) /* Don't go out of bounds */
                continue;

            if (pos(x, y) != '#' && pos(x, y) != 'O') /* Skip walls and visited */
                add_tail(i, x, y, q->steps + 1);
        }
        free(q);
    }

    while ((q = pop_head())) /* Empty the queue */
        free(q);

    return steps;
}

int main(int argc, char **argv)
{
    if (argc != 2) {
        printf("%s <input.txt>\n", argv[0]);
        exit(1);
    }

    FILE *fp = fopen(argv[1], "r");
    int w, h, n;
    
    /* Hack to make it easier to switch between sample and real input */
    /* add "71x71 1024" to the real input and "7x7 12" to the sample input */
    fscanf(fp, "%dx%d %d\n", &w, &h, &n);
    int bytes[10000][2];
    int m = 0;
    while (!feof(fp)) {
        fscanf(fp, "%d,%d\n", &bytes[m][0], &bytes[m][1]);
        m++;
    }

    printf("Read %d 'bytes'\n", m);

    char *buf = calloc(w * h, 1);
    memset(buf, '.', w * h);

    for (int i = 0; i < n; i++)
        pos(bytes[i][0], bytes[i][1]) = '#';

    uint32_t steps = run_bfs(buf, w, h, 0, 0);

    /* Print map */
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++)
            switch (pos(x,y)) {
                case 'O': printf("\033[33;1mO"); break;
                case '#': printf("\033[0;34m#"); break;
                case '.': printf("\033[31;1m."); break;
            }
        printf("\033[0m\n");
    }
    
    printf("Steps: %d\n", steps);
}
