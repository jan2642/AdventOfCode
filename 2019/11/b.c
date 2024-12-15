#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>

typedef struct {
	int64_t  program[1024 * 16];
	uint64_t base;
	uint64_t pc;
	int64_t  in;
	int64_t  out;
} state;

int shiftdec(int a, int n)
{
	while (n--) a /= 10;
	return a;
}

static int64_t arg_ref(state * s, int arg)
{
	int64_t ret = -1;

	switch (shiftdec(s->program[s->pc], arg + 1) % 10) {
		case 0: return s->program[s->pc + arg];
		case 1: return s->pc + arg;
		case 2: return s->base + s->program[s->pc + arg];
		default:
			printf("Unknown attribute!\n");
			exit(1);
	}
}

int64_t run(state * s)
{
	do {
		int64_t a1 = 0, a2 = 0, a3 = 0;

		switch (s->program[s->pc] % 100) { /* Gather arguments */
			case 1: case 2: case 5: case 6: case 7: case 8:
				a1 = s->program[arg_ref(s, 1)];
				a2 = s->program[arg_ref(s, 2)];
				a3 = arg_ref(s, 3);
				break;
			case 3: a1 = arg_ref(s, 1); break;
			case 4: case 9:
				a1 = s->program[arg_ref(s, 1)];
				break;
		}

		// printf("%4lld - %6lld %2lld %9lld %9lld %9lld\n", s->pc, s->program[s->pc], s->program[s->pc] % 100, a1, a2, a3);

		switch (s->program[s->pc] % 100) { /* Execute opcode */
		  /* ADD  */	case 1: s->program[a3] = a1 + a2; s->pc += 4; break;
		  /* MUL  */	case 2: s->program[a3] = a1 * a2; s->pc += 4; break;
		  /* IN   */	case 3: s->program[a1] = s->in; s->pc += 2; break; //return 2;
		  /* OUT  */	case 4: s->out = a1; s->pc += 2; return 1;
		  /* JNZ  */	case 5: if (a1 != 0) s->pc = a2; else s->pc += 3;  break;
		  /* JZ   */	case 6: if (a1 == 0) s->pc = a2; else s->pc += 3;  break;
		  /* LT   */	case 7: s->program[a3] = (a1 < a2) ? 1 : 0; s->pc += 4; break;
		  /* EQ   */	case 8: s->program[a3] = (a1 == a2) ? 1 : 0; s->pc += 4; break;
		  /* BASE */	case 9: s->base += a1; s->pc += 2; break;
		  /* EOP  */	case 99: printf("Reached the end\n"); return 0;
				default:
				printf("Unknown opcode @ %lld : %lld\n", s->pc, s->program[s->pc]);
				return -1;
		}
	}
	while (1);

	return 0;
}

typedef struct point point;

struct point {
	int x, y;
	int color;
	int painted;
	point * next;
};

static point * plist = NULL;

point * get_point(int x, int y)
{
	for (point * p = plist; p; p = p->next)
		if (p->x == x && p->y == y)
			return p;

	point * p = calloc(1, sizeof(point));
	p->x = x;
	p->y = y;
	p->next = plist;
	plist = p;
	return p;
}

int main(int argc, char ** argv)
{
	FILE *f = fopen("input.txt", "r");
	char buf[1024 * 32];
	state state = { 0 };
	int max_pos = 0;
	char * token, * s;

	s = fgets(buf, sizeof(buf), f);
	while ((token = strsep(&s, ",")) != NULL)
		state.program[max_pos++] = atoll(token);
	fclose(f);

	point * pos = get_point(0, 0); /* Start position */
	pos->color = 1; /* Start on white */ 
	int dir = 0; /* Start facing up */

	while (1) {	
		state.in = pos->color;
		if (run(&state) == 0) break;
		pos->color = state.out;
		pos->painted = 1;

		if (run(&state) == 0) break;
		if (state.out == 0) dir--; else dir++; /* Turn CCW or CW ? */
		dir &= 0x3; /* <0 -> +4, >3 -> -4 */

		switch (dir) {
			case 0: pos = get_point(pos->x, pos->y - 1); break; /* Up */
			case 1: pos = get_point(pos->x + 1, pos->y); break; /* Right */
			case 2: pos = get_point(pos->x, pos->y + 1); break; /* Left */
			case 3: pos = get_point(pos->x - 1, pos->y); break; /* Down */
		}
	}

	int min_x = 0, min_y = 0, max_x = 0, max_y = 0;
	for (point * p = plist; p; p = p->next) { /* Find bounding box */
		if (p->x > max_x) max_x = p->x;
		if (p->y > max_y) max_y = p->y;
		if (p->x < min_x) min_x = p->x;
		if (p->y < min_y) min_y = p->y;
	}

	printf("(%d,%d) - (%d,%d)\n", min_x, min_y, max_x, max_y);

	for (int y = min_y; y <= max_y; y++) { /* Paint the picture */
		for (int x = min_x; x <= max_x; x++)
			printf("%s", get_point(x,y)->color ? "##" : "  ");
		printf("\n");
	}

	return 0;
}
