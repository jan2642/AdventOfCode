#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

typedef struct {
	int program[1024 * 32];
	int pc;
	int in;
	int out;
} state;

int shiftdec(int a, int n)
{
	while (n--) a /= 10;
	return a;
}

int run(state * s)
{
	do {
		int a1 = 0, a2 = 0, a3 = 0;

		switch (s->program[s->pc] % 100) {
			case 1: case 2: case 5: case 6: case 7: case 8:
				a1 = (shiftdec(s->program[s->pc], 2) % 10) ? s->program[s->pc + 1] : s->program[s->program[s->pc + 1]];
				a2 = (shiftdec(s->program[s->pc], 3) % 10) ? s->program[s->pc + 2] : s->program[s->program[s->pc + 2]];
				a3 = s->program[s->pc + 3];
				break;
			case 4: a1 = (shiftdec(s->program[s->pc], 2) % 10) ? s->program[s->pc + 1] : s->program[s->program[s->pc + 1]];
				break;
		}

		printf("%4d - %6d %2d %9d %9d %9d\n", s->pc, s->program[s->pc], s->program[s->pc] % 100, a1, a2, a3);  

		switch (s->program[s->pc] % 100) {
			case 1: s->program[a3] = a1 + a2; s->pc += 4; break;
			case 2: s->program[a3] = a1 * a2; s->pc += 4; break;
			case 3: s->program[s->program[s->pc + 1]] = s->in; s->pc += 2; return 2;
			case 4: printf("Output: %d\n", a1); s->out = a1; s->pc += 2; return 1;
			case 5: if (a1 != 0) s->pc = a2; else s->pc += 3;  break;
			case 6: if (a1 == 0) s->pc = a2; else s->pc += 3;  break;
			case 7: s->program[a3] = (a1 < a2) ? 1 : 0; s->pc += 4; break;
			case 8: s->program[a3] = (a1 == a2) ? 1 : 0; s->pc += 4; break;
			case 99: printf("Reached the end\n"); return 0;
			default:
				printf("Wrong os->pcode @ %d : %d\n", s->pc, s->program[s->pc]);
				return -1;
		}
	}
	while (1);

	return 0;
}

static int program[1204 * 32];
static int max_pos = 0;
static int result = 0;

void go(char *s)
{
	state state[5];
	int out = 0;
	memset(state, 0, sizeof(state));
	for (int i = 0; i < strlen(s); i++) {
		int phase = s[i] - '0';
		memcpy(state[i].program, program, max_pos * sizeof(int));
		state[i].in = phase;
		out = run(&state[i]);
		printf("STEP 1: %d %d\n", out, state[i].out);
	}
	int value = 0;
	int i = 0;
	do {
		state[i % 5].in = value;
		out = run(&state[i % 5]);
		if (out == 0) break;
		if (out != 2) printf("NO INPUT? %d\n", out);
		out = run(&state[i % 5]);
		value = state[i % 5].out;
		if (out != 1) printf("NO OUTPUT? %d\n", out);
		i++;
	}
	while(out != 0);
	
	if (value > result)
		result = value;
}

void swap(char * x, char * y)
{ 
	char temp; 
	temp = *x; 
	*x = *y; 
	*y = temp; 
} 
  
void permute(char * a, int l, int r)
{ 
	int i; 
	if (l == r) {
		go(a); 
	}
	else { 
		for (i = l; i <= r; i++) { 
			swap((a + l), (a + i)); 
			permute(a, l + 1, r); 
			swap((a + l), (a + i));
		} 
	} 
} 
  
int main(int argc, char ** argv)
{
	FILE *f = fopen("input.txt", "r");

	char buf[1024 * 32];
	char * s;
	char * token;

	memset(program, 0, sizeof(program));
	s = fgets(buf, sizeof(buf), f);

	while ((token = strsep(&s, ",")) != NULL)
		program[max_pos++] = atoi(token);

	fclose(f);

	char str[] = "56789"; 
	int n = strlen(str); 
	permute(str, 0, n - 1); 

	printf("%d\n", result);

	return 0;
}
