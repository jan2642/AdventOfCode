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
	int depth;
	int level;
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

void add_tail(int dir, int x, int y, int level, int depth)
{
	queue * q = calloc(1, sizeof(queue));
	q->dir = dir;
	q->depth = depth;
	q->x = x;
	q->y = y;
	q->level = level;
	if (qtail)
		qtail->next = q;
	else
		qhead = q;
	qtail = q;
}

typedef struct portal portal;

struct portal {
	char name[3];
	int x[2], y[2], dir[2];
	portal * next;
};

portal * phead;

portal * find_portal_name(char * name)
{
	for (portal * p = phead; p; p = p->next)
		if (strcmp(name, p->name) == 0)
			return p;
	return NULL;
}

int find_portal_dest(int x, int y, int *dx, int *dy, int *dir)
{
	for (portal * p = phead; p; p = p->next) {
		if ((p->x[0] == x) && (p->y[0] == y)) {
			*dx = p->x[1]; *dy = p->y[1]; *dir = p->dir[1];
			return 1;
		}
		if ((p->x[1] == x) && (p->y[1] == y)) {
			*dx = p->x[0]; *dy = p->y[0]; *dir = p->dir[0];
			return 1;
		}
	}
	return 0;
}

void add_portal(char * name, int x, int y, int dir)
{
	//printf("add portal %s @ %d,%d   %d\n", name, x, y, dir);
	portal * p = find_portal_name(name);
	if (!p) {
		p = calloc(1, sizeof(portal));
		strcpy(p->name, name);
		p->next = phead;
		phead = p;
	}

	if (p->x[0] == 0 && p->y[0] == 0) {
		p->x[0] = x; p->y[0] = y; p->dir[0] = dir;
		return;
	}
	if (p->x[1] == 0 && p->y[1] == 0) {
		p->x[1] = x; p->y[1] = y; p->dir[1] = dir;
		return;
	}
	printf("Too many coordinates for portal %s!\n", name);
	exit(1);
}

static int back[] = { 2, 3, 0, 1 };

int run_bfs(char * buf, int w, int h, int x, int y, int dir)
{
	add_tail(dir, x, y, 0, -1);
	queue * q;
	int i = 0;
	int depth;
	portal * end = find_portal_name("ZZ");

	while ((q = pop_head())) {
		depth = q->depth + 1;
		int level = q->level;
		char c = buf[(q->y * w) + q->x];

		if (level >= 30) {
			free(q);
			continue;
		}

		//buf[(q->y * w) + q->x] = 'x';

		if ((end->x[0] == q->x) && (end->y[0] == q->y) && (level == 0))
			break; /* Found the exit */

		for (int i = 0; i < 4; i++) {
			if (q->dir == back[i])
				continue; /* Don't go where we came from */

			x = q->x;
			y = q->y;

			switch (i) {
				case 0: y--; break;
				case 1: x++; break;
				case 2: y++; break;
				case 3: x--; break;
			}

			if (buf[(y * w) + x] == '*') {
				int outer = 0;
				if ((x <= 3) || (x >= (w - 3)) || (y <= 3) || (y >= (h - 3))) outer = 1;
				if ((level == 0) && (outer == 1))
					continue; /* Hit a 'wall', outer portals not accessible at level 0 */

				if (!find_portal_dest(q->x, q->y, &x, &y, &dir)) {
					printf("Could not find other side!\n"); exit(1);
				}
				//printf("Hit a portal at %d,%d -> %d,%d  %d    %d\n", q->x, q->y, x, y, dir, level);
				add_tail(dir, x, y, (outer ? level - 1 : level + 1), depth);
			}
			else if (buf[(y * w) + x] == '.') {
				add_tail(i, x, y, level, depth);
			}
		}
		free(q);
	}

	return depth;
}

int main(int argc, char ** argv)
{
	FILE *f = fopen("input.txt", "r");
	char buf[1024 * 32];
	int x = 0, y = 0, w = 0, h = 0;

    sleep(3);

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

	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			char c = buf[(y * w) + x];
			if (c >= 'A' && c <= 'Z') {
				char name[3] = { 0 };
				name[0] = c;
				name[1] = buf[(y * w) + x + 1];
				if (name[1] < 'A' || name[1] > 'Z')
					continue;

				int dir = 3;
				if (x < 4) dir = 1;
				else if (x < (w / 2)) dir = 3;
				else if (x < (w - 4)) dir = 1;

				//printf("(H) found portal: %s %d\n", name, dir);
				add_portal(name, (dir == 1 ? x + 2 : x - 1), y, dir);
				buf[(y * w) + x] = '*';
				buf[(y * w) + x + 1] = '*';
				x++;
			}
		}
	}
	for (int x = 0; x < w; x++) {
		for (int y = 0; y < h; y++) {
			char c = buf[(y * w) + x];
			if (c >= 'A' && c <= 'Z') {
				char name[3] = { 0 };
				name[0] = c;
				name[1] = buf[((y + 1) * w) + x];
				if (name[1] < 'A' || name[1] > 'Z')
					continue;

				int dir = 0;
				if (y < 4) dir = 2;
				else if (y < (h / 2)) dir = 0;
				else if (y < (h - 4)) dir = 2;

				//printf("(V) found portal: %s %d\n", name, dir);
				add_portal(name, x, (dir == 2 ? y + 2 : y - 1), dir);
				buf[(y * w) + x] = '*';
				buf[((y + 1) * w) + x] = '*';
				y++;
			}
		}
	}

	portal * begin = find_portal_name("AA");
	printf("Start at %d,%d\n", begin->x[0], begin->y[0]);
	int depth = run_bfs(buf, w, h, begin->x[0], begin->y[0], begin->dir[0]);
#if 0
	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			printf("%c", buf[(y * w) + x]);
		}
		printf("\n");
	}
#endif
	printf("Depth: %d\n", depth);

	return 0;
}
