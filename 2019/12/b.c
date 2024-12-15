#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

typedef struct moon moon;

struct moon {
	int p[3]; /* position */
	int v[3]; /* velocity */
	int g[3]; /* gravity */
	int i[3]; /* initial position */
	moon * next;
};

static moon * mlist = NULL;

void add_moon(int x, int y, int z)
{
	moon * m;
	m = calloc(1, sizeof(moon));
	m->p[0] = m->i[0] = x;
	m->p[1] = m->i[1] = y;
	m->p[2] = m->i[2] = z;
	m->next = mlist;
	mlist = m;
}

int64_t gcd(int64_t a, int64_t b)
{
	if (a == 0) return b;
	return gcd(b % a, a);
}

int64_t lcm(int64_t a, int64_t b)
{
	return (a * b) / gcd(a, b);
}

int main(int argc, char ** argv)
{
	FILE *f = fopen("input.txt", "r");
	int x, y, z;
	int moons = 0;

	while (fscanf(f, "<x=%d, y=%d, z=%d>\n", &x, &y, &z) > 0) {
		add_moon(x, y, z);
		moons++;
	}

	fclose(f);

	int per[3];
       	per[0] = per[1] = per[2] = -1;

	for (int step = 0; ; step++) {
		for (moon * m = mlist; m; m = m->next) {
			for (int i = 0; i < 3; i++) {
				m->v[i] += m->g[i]; /* Update velocity */
				m->p[i] += m->v[i]; /* Update position */
				m->g[i] = 0;        /* Clear gravity */
			}
		}

		for (int i = 0; i < 3; i++) { /* Detect period (per dimension) */
			if ((per[i] != -1) || (step == 0)) continue;
			int match = 0;
			for (moon * m = mlist; m; m = m->next)
				if ((m->p[i] == m->i[i]) && (m->v[i] == 0))
					match++; /* Pos & Vel match with initial */
			if (match == moons)
				per[i] = step; /* All moons match: found it! */
		}
		
		if (per[0] >= 0 && per[1] >= 0 && per[2] >= 0)
			break; /* All periods found, time to quit */

		for (moon * m = mlist; m->next; m = m->next) { /* Calculate gravity */
			for (moon * p = m->next; p; p = p->next) {
				for (int i = 0; i < 3; i++) {
					if (m->p[i] < p->p[i]) { m->g[i]++; p->g[i]--; }
					if (m->p[i] > p->p[i]) { m->g[i]--; p->g[i]++; }
				}
			}
		}
	}

	int64_t period = 1;
	for (int i = 0; i < 3; i++)
		period = lcm(period, per[i]);
	printf("Period: %lld steps\n", period);
}
