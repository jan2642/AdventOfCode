#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>

typedef struct {
	int64_t  program[1024 * 2];
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

typedef struct queue queue;

struct queue {
	state * state;
	int dir;
	int depth;
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

void add_tail(state * state, int dir, int depth)
{
	queue * q = calloc(1, sizeof(queue));
	q->state = state;
	q->dir = dir;
	q->depth = depth;
	if (qtail)
		qtail->next = q;
	else
		qhead = q;
	qtail = q;
}

void clear_queue()
{
	queue * q;
	while ((q = pop_head())) free(q);
}

static int back[] = { 0, 2, 1, 4, 3 }; 
int run_bfs(state * st, state ** oxygen)
{
	add_tail(st, 0, 0);
	queue * q;
	int depth;	
	while ((q = pop_head())) {
		depth = q->depth;
		for (int i = 1; i <= 4; i++) {
			if (q->dir == back[i]) /* Don't go where we came from */
				continue;
			state * ns = clone_state(q->state);
			ns->in = i; /* Direction */
			run(ns); /* Move it */
			if (oxygen && ns->out == 2) {
				*oxygen = ns;
				return q->depth + 1;
			}
			else if (ns->out == 1) {
				add_tail(ns, i, q->depth + 1);
			}
			else {
				free(ns);
			}
		}
		free(q->state);
		free(q);
	}

	return depth;
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

	state * oxygen;
	printf("Found oxygene! %d\n", run_bfs(st, &oxygen));

	clear_queue();
	printf("Time to fill: %d\n", run_bfs(oxygen, NULL));

	return 0;
}
