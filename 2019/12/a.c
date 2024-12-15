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
	moon * next;
};

static moon * mlist = NULL;

void add_moon(int x, int y, int z)
{
	moon * m;
	m = calloc(1, sizeof(moon));
	m->p[0] = x;
	m->p[1] = y;
	m->p[2] = z;
	m->next = mlist;
	mlist = m;
}

int main(int argc, char ** argv)
{
	FILE *f = fopen("input.txt", "r");
	int x, y, z;

	while (fscanf(f, "<x=%d, y=%d, z=%d>\n", &x, &y, &z) > 0)
		add_moon(x, y, z);

	fclose(f);

	for (int step = 0; step <= 1000; step++) {
		for (moon * m = mlist; m; m = m->next) {
			for (int i = 0; i < 3; i++) {
				m->v[i] += m->g[i];
				m->p[i] += m->v[i];
				m->g[i] = 0;
			}
		}

		for (moon * m = mlist; m->next; m = m->next) {
			for (moon * p = m->next; p; p = p->next) {
				for (int i = 0; i < 3; i++) {
					if (m->p[i] < p->p[i]) { m->g[i]++; p->g[i]--; }
					if (m->p[i] > p->p[i]) { m->g[i]--; p->g[i]++; }
				}
			}
		}
	}
		
       	int pot, kin, energy = 0;
	for (moon * m = mlist; m; m = m->next) {
		pot = 0; kin = 0;
		for (int i = 0; i < 3; i++) {
			pot += abs(m->p[i]);
			kin += abs(m->v[i]);
		}
		energy += pot * kin; 
	}

	printf("Energy: %d\n", energy);

}
