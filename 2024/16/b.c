#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>

typedef struct queue queue;

struct queue {
    int dir;
    int x, y;
    uint32_t score;
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

void add_tail(int dir, int x, int y, uint32_t score)
{
    queue * q = calloc(1, sizeof(queue));
    q->dir = dir;
    q->x = x;
    q->y = y;
    q->score = score;
    if (qtail)
        qtail->next = q;
    else
        qhead = q;
    qtail = q;
}

static int back[] = { 2, 3, 0, 1 };
static int directions[4][2] = { { 0, -1 }, /* up */ { 1, 0 }, /* right */ { 0, 1 }, /* down */ { -1, 0 } /* left */ };

#define pos(x, y) buf[((y) * w) + (x)]   /* Character at x,y */
#define score(x, y, dir) scores[((((y) * w) + (x)) * 4) + dir]  /* Current score at x,y,dir */

uint32_t run_bfs(char * buf, uint32_t * scores, int w, int h, int x, int y, int dir)
{
    /* Add the start position to the queue */
    add_tail(dir, x, y, 0);

    queue * q;
    uint32_t best_score = 0xffffffff;

    while ((q = pop_head())) {
        if (score(q->x, q->y, q->dir) < q->score) {
            /* Position has been reached already with a lower score */
            free(q);
            continue;
        }

        score(q->x, q->y, q->dir) = q->score;

        if (pos(q->x, q->y) == 'E') {
            /* Found the exit, keep looking for better scores */
            if (best_score > q->score)
                best_score = q->score;
            free(q);
            continue;
        }

        /* Look in every direction */
        for (int i = 0; i < 4; i++) {
            if (q->dir == back[i])
                continue; /* Don't go where we came from */

            x = q->x + directions[i][0];
            y = q->y + directions[i][1];

            if (pos(x, y) != '#') {
                if (q->dir == i) /* Same direction, step */
                    add_tail(i, x, y, q->score + 1);
                else /* Other direction, turn */
                    add_tail(i, q->x, q->y, q->score + 1000);
            }
        }
        free(q);
    }

    return best_score;
}

uint32_t run_bfs_2(char * buf, uint32_t * scores, int w, int h, int x, int y)
{
    /* Add the start position to the queue: find the dir with the lowest score */
    uint32_t s = 0xffffffff;
    uint32_t dir;
    for (int i = 0; i < 4; i++)
        if (score(x, y, i) < s) {
            s = score(x, y, i);
            dir = i;
        }
    printf("Start in dir %d with score %u\n", dir, s);
    add_tail(dir, x, y, s);

    queue * q;
    uint32_t tiles = 0;

    /* Trace back from the end to start, only following valid score paths */
    while ((q = pop_head())) {
        if (score(q->x, q->y, q->dir) != q->score) { /* Invalid move */
            free(q);
            continue;
        }

        /* Mark & count visited */
        if (pos(q->x, q->y) != 'O') {
            tiles++;
            pos(q->x, q->y) = 'O';
        }

        if (q->score == 0) { /* Reached the start */
            free(q);
            continue;
        }

        /* Look in every direction */
        for (int i = 0; i < 4; i++) {
            if (q->dir == back[i])
                continue; /* Don't go where we came from */

            x = q->x + directions[back[i]][0];
            y = q->y + directions[back[i]][1];

            if (pos(x, y) != '#') {
                if (q->dir == i) /* Step (backwards) */
                    add_tail(i, x, y, q->score - 1);
                else
                    add_tail(i, q->x, q->y, q->score - 1000);
            }
        }
        free(q);
    }
    return tiles;
}

int main(int argc, char ** argv)
{
    if (argc != 2) {
        printf("%s <input.txt>\n", argv[0]);
        exit(1);
    }

    FILE *f = fopen(argv[1], "r");
    char buf[1024 * 32];
    int x = 0, y = 0, w = 0, h = 0, sx = 0, sy = 0, ex = 0, ey = 0;

    while (!feof(f)) {
        char c = fgetc(f);
        if (c == '\n') {
            if (w == 0) w = x;
            x = 0; y++;
            continue;
        }
        buf[(y * w) + x] = c;
        if (c == 'S') { sx = x; sy = y; }
        if (c == 'E') { ex = x; ey = y; }
        x++;
    }
    h = y;
    fclose(f);

    printf("%d x %d\n", w, h);

    /* Part 1: Start at S and find the minimal score path(s) */

    uint32_t *scores = calloc(w * h * 4, sizeof(*scores));
    memset(scores, 0xff, w * h * 4 * sizeof(*scores));
    uint32_t score = run_bfs(buf, scores, w, h, sx, sy, 1); /* Start facing right */

    /* Part 2: Start at E and follow the 'valid score' paths */

    uint32_t tiles = run_bfs_2(buf, scores, w, h, ex, ey);
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            printf("%c", pos(x, y));
        }
        printf("\n");
    }

    printf("Part 1: Score: %u\n", score);
    printf("Part 2: Tiles: %u\n", tiles);

    return 0;
}
