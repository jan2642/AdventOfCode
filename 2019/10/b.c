#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

typedef struct point point;

struct point{
	double x;
	double y;
	double dist;
	double angle;
	int zapped;
	point * next;
};

double between(double a, double b1, double b2)
{
	if ((a >= b1 && a <= b2) || (a <= b1 && a >= b2))
		return 1;
	return 0;
}

int compare(const void * vp, const void * vq)
{
	point * p = (point *)vp;
	point * q = (point *)vq;

	/* First order by angle, then order by distance */
	if (p->angle < q->angle) return -1;
	if (p->angle > q->angle) return 1;
	if (p->dist < q->dist) return -1;
	if (p->dist > q->dist) return 1;
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
	point * center = NULL;

	while ((s = fgets(buf, sizeof(buf), f))) {
		int x = 0;

		for (; *s; s++, x++) {
			if (*s == '#' || *s == 'X') {
				point * p = calloc(1, sizeof(point));
				p->x = x;
				p->y = y;
				p->next = list;
				list = p;
				if (*s == 'X')
					center = p;
			}
		}
		y++;
	}

	int total = 0;
	int max_hit = 0;
	for (point * p = list; p; p = p->next, total++) {
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
		if (hit > max_hit) {
			max_hit = hit;
			center = p;
		}
		//printf("%d : (%.0f,%.0f) : %d\n", i++, p->x, p->y, hit);
	}
	
	printf("center: %d (%.0f,%.0f)\n", max_hit, center->x, center->y);

	point * all = calloc(1, total * sizeof(point)); /* Array which can be sorted with qsort() */
	int i = 0;
	for (point * p = list; p; p = p->next) {
		if (p == center) continue;
		all[i].x = p->x;
		all[i].y = p->y;
		all[i].dist = (p->x - center->x) * (p->x - center->x) + (p->y - center->y) * (p->y - center->y);
		all[i].angle = atan2(p->y - center->y, p->x - center->x) + M_PI/2;
		if (all[i].angle > 2 * M_PI) all[i].angle -= 2 * M_PI;
		if (all[i].angle < 0) all[i].angle += 2 * M_PI;
		i++;
	}
	qsort(all, total - 1, sizeof(point), compare);

	int zapped = 1;
	while (zapped < 200) {
		for (i = 0; i < total - 1; i++, zapped++) {
			if (all[i].zapped) continue;
			double angle = all[i].angle;
			all[i].zapped = 1;
			// printf("%d - %d - (%.0f,%.0f) %f %f    %f %f %d\n", zapped, i, all[i].x, all[i].y, all[i].angle, all[i].dist, all[i + 1].angle, angle, all[i + 1].angle == angle);
			if (zapped == 200) {
				printf("%.0f\n", all[i].x * 100 + all[i].y);
				exit(0);
			}
			while (angle == all[i + 1].angle) i++; /* Skip all points with the same angle until the next sweep */
		}
	}

	free(all);
	fclose(f);
}
