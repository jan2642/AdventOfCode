#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>

#define DISPLAY

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

#define EMPTY  (0)
#define WALL   (1)
#define BLOCK  (2)
#define PADDLE (3)
#define BALL   (4)

int main(int argc, char ** argv)
{
	FILE *f = fopen("input.txt", "r");
	char buf[1024 * 32];
	state state = { 0 };
	int max_pos = 0;
	char * token, * s;
	int display = 1;

	s = fgets(buf, sizeof(buf), f);
	while ((token = strsep(&s, ",")) != NULL)
		state.program[max_pos++] = atoll(token);
	fclose(f);

	state.program[0] = 2; /* 'Play for free' */

	if (display) {
		setvbuf(stdout, NULL, _IONBF, 0);
		printf("\033[2J");
	}

	int score = 0;
	int ball_x = 0;
	int paddle_x = 0;
	while (1) {
		if (run(&state) == 0) break; /* get x */
		int x = state.out;
		if (run(&state) == 0) break; /* get y */
		int y = state.out;
		if (run(&state) == 0) break; /* get type */
		int type = state.out;

		if ((x == -1) && (y == 0)) {
			score = type;
			if (display)
				printf("\033[%d;%dH\033[0mScore: %d  ", 1, 3, score);
			continue;
		}

		if (type == BALL) ball_x = x;
		if (type == PADDLE) paddle_x = x;

		state.in = 0; /* Don't move */
		if (ball_x > paddle_x) state.in = 1; /* Move right */
		if (ball_x < paddle_x) state.in = -1; /* Move left */

		if (display) {
			static char * tile[] = { " ", "\033[0;34m#", "\033[31;1m*", "\033[37;1m-", "\033[33;1mO" };
			printf("\033[%d;%dH%s\033[1;1H", y + 2, x + 1,
					tile[type]);
			fsync(1);
			usleep(5000);
			//usleep(500);
		}
	}

	if (display) printf("\033[%d;%dH", 25, 1); /* Jump below the display */
	printf("Score: %d\n", score);

	return 0;
}
