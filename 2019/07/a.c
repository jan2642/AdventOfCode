#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int shiftdec(int a, int n)
{
	while (n--) a /= 10;
	return a;
}

int run(int * program, int in1, int in2)
{
	int pc = 0;
	int ic = 0;
	char buf[32];

	do {
		int a1 = 0, a2 = 0, a3 = 0;

		switch (program[pc] % 100) {
			case 1: case 2: case 5: case 6: case 7: case 8:
				a1 = (shiftdec(program[pc], 2) % 10) ? program[pc + 1] : program[program[pc + 1]];
				a2 = (shiftdec(program[pc], 3) % 10) ? program[pc + 2] : program[program[pc + 2]];
				a3 = program[pc + 3];
				break;
			case 4: a1 = (shiftdec(program[pc], 2) % 10) ? program[pc + 1] : program[program[pc + 1]];
				break;
		}

		printf("%4d - %6d %2d %9d %9d %9d\n", pc, program[pc], program[pc] % 100, a1, a2, a3);  

		switch (program[pc] % 100) {
			case 1: program[a3] = a1 + a2; pc += 4; break;
			case 2: program[a3] = a1 * a2; pc += 4; break;
			case 3: program[program[pc + 1]] = (ic == 0 ? in1 : in2); ic++; pc += 2; break;
			case 4: printf("Output: %d\n", a1); return a1;
			case 5: if (a1 != 0) pc = a2; else pc += 3;  break;
			case 6: if (a1 == 0) pc = a2; else pc += 3;  break;
			case 7: program[a3] = (a1 < a2) ? 1 : 0; pc += 4; break;
			case 8: program[a3] = (a1 == a2) ? 1 : 0; pc += 4; break;
			case 99: printf("Reached the end: %d %d\n", in1, in2); return -1;
			default:
				printf("Wrong opcode @ %d : %d\n", pc, program[pc]);
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
	int program_test[1204 * 32];
	int out = 0;
	for (int i = 0; i < strlen(s); i++) {
		int phase = s[i] - '0';
		memcpy(program_test, program, max_pos * sizeof(int));
		out = run(program_test, phase, out);
	}
	printf("%d\n", out);
	if (out > result)
		result = out;
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

	char str[] = "01234"; 
	int n = strlen(str); 
	permute(str, 0, n - 1); 

	printf("%d\n", result);

	return 0;
}
