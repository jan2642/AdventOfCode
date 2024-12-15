#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

typedef struct point point;

struct point{
	int x;
	int y;
	point * next;
};

int between(int a, int b1, int b2)
{
	if ((a >= b1 && a <= b2) || (a <= b1 && a >= b2))
		return 1;
	return 0;
}

int main(int argc, char ** argv)
{
	FILE *f = fopen("input.txt", "r");

	char buf[1024];
	char * s;

	int x;
	int y = 0;

	point * list;

	while ((s = fgets(buf, sizeof(buf), f))) {
		int x = 0;

		for (; *s; s++, x++) {
			if (*s == '#' || *s == 'X') {
				point * p = calloc(1, sizeof(point));
				p->x = x;
				p->y = y;
				p->next = list;
				list = p;
			}
		}
		y++;
	}

	int i = 0;
	int max_hit = 0;
	for (point * p = list; p; p = p->next) {
		int hit = 0;
		for (point * q = list; q; q = q->next) {
			if (q == p) continue;
		
			/* Check for blocking points in between */
			point * r = list;
			for (; r; r = r->next) {
				if (r == p || r == q) continue;
				if (!between(r->x, p->x, q->x) || !between(r->y, p->y, q->y)) continue;
				if (p->x == q->x && p->x == r->x) break ; /* On the same column */
				if (p->y == q->y && p->y == r->y) break; /* On the same line */
				if ((p->x - r->x) * (q->y - r->y) == (q->x - r->x) * (p->y - r->y)) break;
			}
			if (r == NULL) hit++;

		}
		if (hit > max_hit) max_hit = hit;
		//printf("%d : %d,%d : %d\n", i++, p->x, p->y, hit);
	}

	printf("%d\n", max_hit);

	fclose(f);
}
