#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

typedef struct {
	int x;
	int y;
} point;

typedef struct {
	point a;
	point b;
} line;

int distance(point * p1, point * p2)
{
	int d = abs(p1->x - p2->x) + abs(p1->y - p2->y);
	return d;
}

int between(int a, int b1, int b2)
{
	if ((a >= b1 && a <= b2) || (a <= b1 && a >= b2))
		return 1;
	return 0;
}

int intersect(line * l1, line * l2, point * i)
{
	if (l1->a.x == l1->b.x) {
		if (l2->a.x == l2->b.x) {
			return 0; /* Both vertical */
		}
		if (between(l2->a.y, l1->a.y, l1->b.y) &&
				between(l1->a.x, l2->a.x, l2->b.x)) {
			i->x = l1->a.x;
			i->y = l2->a.y;
			printf("(%d,%d)-(%d,%d) & (%d,%d)-(%d,%d) -> ",
					l1->a.x, l1->a.y, l1->b.x, l1->b.y,
					l2->a.x, l2->a.y, l2->b.x, l2->b.y);
			printf("(%d,%d)\n", i->x, i->y);
			return 1;
		}
	}
	if (l1->a.y == l1->b.y) {
		if (l2->a.y == l2->b.y) {
			return 0; /* Both horizontal */
		}
		if (between(l1->a.y, l2->a.y, l2->b.y) &&
				between(l2->a.x, l1->a.x, l1->b.x)) {
			i->x = l2->a.x;
			i->y = l1->a.y;
			printf("(%d,%d)-(%d,%d) & (%d,%d)-(%d,%d) -> ",
					l1->a.x, l1->a.y, l1->b.x, l1->b.y,
					l2->a.x, l2->a.y, l2->b.x, l2->b.y);
			printf("(%d,%d)\n", i->x, i->y);
			return 1;
		}
	}

	return 0;
}

int gen_path(line * path, char * s)
{
	char * token;
	char dir;
	int distance;
	int i;
	point c = { 0, 0 };

	while ((token = strsep(&s, ",")) != NULL) {
		dir = *token++;
		distance = atoi(token);

		memcpy(&path->a, &c, sizeof(c));

		switch (dir) {
			case 'R': c.x += distance; break;
			case 'L': c.x -= distance; break;
			case 'U': c.y -= distance; break;
			case 'D': c.y += distance; break;
			default:
				printf("Error: %c\n", dir);
				exit(1);
		}

		memcpy(&path->b, &c, sizeof(c));

		path++;
		i++;
	}

	return i;
}

int main(int argc, char ** argv)
{
	FILE *f = fopen("input.txt", "r");

	char buf[1024 * 8];
	char * s;
	line p1[1024];
	line p2[1024];
	int l1, l2;

	s = fgets(buf, sizeof(buf), f);
	l1 = gen_path(p1, s);
	s = fgets(buf, sizeof(buf), f);
	l2 = gen_path(p2, s);

	fclose(f);

	point p = { 999999, 999999 };
	point c = { 0, 0 };
	int d, dist = distance(&p, &c);
	for (int i = 0; i < l1; i++) {
		for (int j = 0; j < l2; j++) {
			if (intersect(&p1[i], &p2[j], &p)) {
				d = distance(&c, &p);
				if (d < dist && p.x != 0 && p.y != 0)
					dist = d;
			}
		}
	}

	printf("%d %d -> %d\n", l1, l2, dist);
}
