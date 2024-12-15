#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>

typedef struct {
	int64_t  program[1024 * 8];
	uint64_t base;
	uint64_t pc;
	int64_t  in;
	int64_t  out;
	int64_t  max_pos;
} state;

state * clone_state(state * s) {
	state * r = malloc(sizeof(state));
	memcpy(r, s, sizeof(state));
	return r;
}

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

int main(int argc, char ** argv)
{
	FILE *f = fopen("input.txt", "r");
	char buf[1024 * 32];
	state * st = calloc(1, sizeof(state));
	int max_pos = 0;
	char * token, * s;

	s = fgets(buf, sizeof(buf), f);
	while ((token = strsep(&s, ",")) != NULL)
		st->program[max_pos++] = atoll(token);
	st->max_pos = max_pos;
	fclose(f);

	int w = 0, h = 0;
	int x = 0, y = 0;
	while (1) {
		if (run(st) == 0) break;
		char c = (char) st->out;
		buf[(y * w) + x] = c;

		if (c == '\n') {
			if (w == 0) w = x;
			x = 0;
			y++;
			continue;
		}
		x++;
	}
	h = y - 1;

	printf("%d x %d\n", w, h);

	int alignment = 0;
	for (y = 1; y < h - 1; y++)
		for (x = 1; x < w - 1; x++) {
			if ((buf[(y * w) + x] != '.') &&
			    (buf[(y * w) + (x - 1)] != '.') && 
			    (buf[(y * w) + (x + 1)] != '.') && 
			    (buf[((y - 1) * w) + x] != '.') && 
			    (buf[((y + 1) * w) + x] != '.')) {
			       alignment += x * y;
			       buf[(y * w) + x] = 'O'; 
			}
		}

	for (y = 0; y < h; y++) {
		for (x = 0; x < w; x++) {
			printf("%c", buf[(y * w) + x]);
			printf("%c", buf[(y * w) + x]);
		}
		printf("\n");
	}
	printf("alignment: %d\n", alignment);

	return 0;
}
