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
		  /* IN   */	case 3: s->program[a1] = s->in; s->pc += 2; return 2;
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

int64_t run2(state * s, int64_t in)
{
	int res;
	s->in = in;
	while ((res = run(s)) == 1) {
		printf("%c", (char)s->out);
	}
	return res;
}

int64_t run_str(state * s, char * str)
{
	int res;
	while (*str)
		res = run2(s, *str++);
	return res;
}

char ahead(char *buf, int w, int h, int x, int y, int dir)
{
	dir %= 4;
	if (dir < 0) dir += 4;

	switch (dir) {
		case 0: y--; break;
		case 1: x++; break;
		case 2: y++; break;
		case 3: x--; break;
	}

	return buf[y * w + x];
}

typedef struct queue queue;

struct queue {
	char symbol[400];
	int tagged;
	queue * next;
};

queue * qhead, * qtail;

void add_tail(char * symbol)
{
	queue * q = calloc(1, sizeof(queue));
	strcpy(q->symbol, symbol);
	if (qtail)
		qtail->next = q;
	else
		qhead = q;
	qtail = q;
}

char * get_pair(int offset) {
	queue * q = qhead;
	for (; q && offset; q = q->next, offset--);
	if (offset >= 0 && q && q->next) {
		char buf[400];
		sprintf(buf, "%s,%s", q->symbol, q->next->symbol);
		return strdup(buf);
	}
	return NULL;
}

char * replace_pair(int offset, char * symbol) {
	queue * q = qhead;
	for (; q && offset; q = q->next, offset--);
	if (offset >= 0 && q && q->next) {
		queue * r = q->next;
		strcpy(q->symbol, symbol);
		q->next = r->next;
		free(r);
	}
	return NULL;
}

int main(int argc, char ** argv)
{
	FILE *f = fopen("input.txt", "r");
	char buf[1024 * 32];
	state * st = calloc(1, sizeof(state)), * strun;
	int max_pos = 0;
	char * token, * s;

	s = fgets(buf, sizeof(buf), f);
	while ((token = strsep(&s, ",")) != NULL)
		st->program[max_pos++] = atoll(token);
	st->max_pos = max_pos;
	fclose(f);

	strun = clone_state(st);

	memset(buf, '.', sizeof(buf));
	int w = 0, h = 0;
	int x = 1, y = 1, bx = 0, by = 0, dir = 0;
	while (1) {
		if (run(st) == 0) break;
		char c = (char) st->out;

		if (c == '\n') {
			if (w == 0) w = x + 1;
			x = 1; y++;
			continue;
		}

		buf[(y * w) + x] = c;

		if ((c != '.') && (c != '#')) {
			bx = x; by = y; /* Bot position */
			switch (c) {    /* Bot direction */
				case '^': dir = 0; break;
				case '>': dir = 1; break;
				case 'V': dir = 2; break;
				case '<': dir = 3; break;
			}
		}
		x++;
	}
	h = y;

	/* Fix first line because the width wasn't known yet */
	memcpy(&buf[w], &buf[0], w);
	memset(buf, '.', w);

	x = bx; y = by;
	while (1) {
		char lr = 0;
		if (ahead(buf, w, h, x, y, dir - 1) == '#') { /* Go left */
			lr = 'L';
			dir = (dir - 1) % 4;
			if (dir < 0) dir += 4;
		}
		else if (ahead(buf, w, h, x, y, dir + 1) == '#') { /* Go right */
			lr = 'R';
			dir = (dir + 1) % 4;
		}
		else { /* End of the line */
			break;
		}

		int steps = 0;
		while (ahead(buf, w, h, x, y, dir) != '.') {
			switch (dir) {
				case 0: y--; break;
				case 1: x++; break;
				case 2: y++; break;
				case 3: x--; break;
			}
			steps ++;
		}

		char symbol[16];
		snprintf(symbol, sizeof(symbol), "%c,%d", lr, steps);
		add_tail(symbol);
	}

	for (queue * q = qhead; q; q = q->next) { printf("'%s' ", q->symbol); } printf("\n");

	int unique = 0;
	while (1) {
		char * p = get_pair(0);
		char * max_symbol = strdup(p);
		int max_count = 0;
		for(int i = 0; p; p = get_pair(++i)) {
			char * q = get_pair(0);
			int count = 0;
			for (int j = 0; q; q = get_pair(++j)) {
				if (p == q) continue;
				if (strcmp(p, q) == 0)
					count++;
				free(q);
			}
			if (count >= max_count) {
				max_count = count;
				free(max_symbol);
				max_symbol = strdup(p);
			}	
			//printf("%d %d %s\n", i, count, p);
			free(p);
		}
		//printf("%d %s\n", max_count, max_symbol);
		if (max_count == 1)
			break;
		p = get_pair(0);
		for (int i = 0; p; p = get_pair(++i)) {
			if (strcmp(p, max_symbol) == 0) {
				replace_pair(i, max_symbol);
			}
		}

		for (queue * q = qhead; q; q = q->next) { printf("'%s' ", q->symbol); }

		unique = 0;
		for (queue * q = qhead; q; q = q->next) q->tagged = 0;
		for (queue * q = qhead; q; q = q->next) {
			if (q->tagged == 1) continue;
			unique++;
			for (queue * r = qhead; r; r = r->next)
				if (strcmp(q->symbol, r->symbol) == 0)
					r->tagged = 1;
		}
		printf("  unique: %d\n", unique);
		if (unique <= 3) break;
	}

	char code[3][200];
	for (queue * q = qhead; q; q = q->next) q->tagged = 0;
	for (int i = 0; i < 3; i++) {
		queue * q = qhead;
		for (; q; q = q->next)
			if (q->tagged == 1) continue; else break;

		strcpy(code[i], q->symbol);
		printf("%c %s\n", 'A' + i, code[i]);
			
		for (queue * r = qhead; r; r = r->next)
			if (strcmp(r->symbol, code[i]) == 0) {
				sprintf(r->symbol, "%c", 'A' + i);
				r->tagged = 1;
			}
	}
	
	for (queue * q = qhead; q; q = q->next) { printf("'%s' ", q->symbol); } printf("\n");
	
	strun->program[0] = 2;

	int res;

	for (queue * q = qhead; q; q = q->next) {
	        run_str(strun, q->symbol);
	        run_str(strun, q->next ? "," : "\n");
	}

	for (int i = 0; i < 3; i++) {
		s = code[i];
		run_str(strun, code[i]);
		run_str(strun, "\n");
	}

	run_str(strun, "n\n");

	while (run2(strun, 0) != 0);

	printf("result: %lld\n", strun->out);
	return 0;
}
