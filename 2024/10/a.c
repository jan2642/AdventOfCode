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
    char value;
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

void add_tail(int dir, int x, int y, char value)
{
	queue * q = calloc(1, sizeof(queue));
	q->dir = dir;
	q->x = x;
	q->y = y;
    q->value = value;
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

int run_bfs(char * buf, int w, int h, int x, int y, int dir)
{
    /* Add the start position to the queue */
	add_tail(dir, x, y, buf[(y * w) + x]);

	queue * q;
	int i = 0;
    int routes = 0;

	while ((q = pop_head())) {
		char c = buf[(q->y * w) + q->x];
		
        buf[(q->y * w) + q->x] = 'x'; /* Mark visited */

        if (c == '9') {
            /* Found a top */
            routes++;
            continue;
        }

        /* Look in every direction */
		for (int i = 0; i < 4; i++) {
			x = q->x + directions[i][0];
			y = q->y + directions[i][1];

            /* Don't go out of bounds */
            if (x < 0 || x >= w || y < 0 || y >= h)
                continue;

            if (buf[(y * w) + x] == (q->value + 1)) {
                /* Valid move -> only go up one level */
				add_tail(i, x, y, buf[(y * w) + x]);
			}
		}
		free(q);
	}

	return routes;
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

	while (!feof(f)) {
		char c = fgetc(f);
		if (c == '\n') {
			if (w == 0) w = x;
			x = 0; y++;
			continue;
		}
		buf[(y * w) + x] = c;
		x++;
	}
	h = y;
	fclose(f);

	printf("%d x %d\n", w, h);

    int score = 0;
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
			char c = buf[(y * w) + x];

            if (c == '0') {
                /* Start position found */

                char *work = strdup(buf);
                int routes = run_bfs(work, w, h, x, y, 4);
                free(work);
                
                printf("Entry at %d,%d --> %d\n", x, y, routes);

                score += routes;
            }
        }
    }

	printf("Score: %d\n", score);
	return 0;
}
