#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>

typedef struct {
	int64_t program[1024 * 16];
	uint64_t base;
	int pc;
	int64_t in;
	int64_t out;
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

		switch (s->program[s->pc] % 100) {
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

		//printf("%4d - %6lld %2lld %9lld %9lld %9lld\n", s->pc, s->program[s->pc], s->program[s->pc] % 100, a1, a2, a3);

		switch (s->program[s->pc] % 100) {
			case 1: s->program[a3] = a1 + a2; s->pc += 4; break;
			case 2: s->program[a3] = a1 * a2; s->pc += 4; break;
			case 3: s->program[a1] = s->in; s->pc += 2; break; //return 2;
			case 4: printf("Output: %lld\n", a1); s->out = a1; s->pc += 2; break; //return 1;
			case 5: if (a1 != 0) s->pc = a2; else s->pc += 3;  break;
			case 6: if (a1 == 0) s->pc = a2; else s->pc += 3;  break;
			case 7: s->program[a3] = (a1 < a2) ? 1 : 0; s->pc += 4; break;
			case 8: s->program[a3] = (a1 == a2) ? 1 : 0; s->pc += 4; break;
			case 9: s->base += a1; s->pc += 2; break;
			case 99: printf("Reached the end\n"); return 0;
			default:
				printf("Wrong os->pcode @ %d : %lld\n", s->pc, s->program[s->pc]);
				return -1;
		}
	}
	while (1);

	return 0;
}

static int64_t program[1204 * 32];
static int max_pos = 0;
static int result = 0;

int main(int argc, char ** argv)
{
	FILE *f = fopen("input.txt", "r");

	char buf[1024 * 32];
	char * s;
	char * token;

	memset(program, 0, sizeof(program));
	s = fgets(buf, sizeof(buf), f);

	while ((token = strsep(&s, ",")) != NULL)
		program[max_pos++] = atoll(token);

	fclose(f);
	
	state state = { 0 };
	memset(&state, 0, sizeof(state));
	state.in = 1;
	memcpy(state.program, program, max_pos * sizeof(program[0]));
       	int64_t out = run(&state);

	printf("%lld %lld\n", out, state.out);

	return 0;
}
