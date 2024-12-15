#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int shiftdec(int a, int n)
{
	while (n--) a /= 10;
	return a;
}

int run(int * program)
{
	int pc = 0;
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
			case 3: printf("Input: "); program[program[pc + 1]] = atoi(fgets(buf, sizeof(buf), stdin)); pc += 2; break;
			case 4: printf("Output: %d\n", a1); pc += 2; break;
			case 5: if (a1 != 0) pc = a2; else pc += 3;  break;
			case 6: if (a1 == 0) pc = a2; else pc += 3;  break;
			case 7: program[a3] = (a1 < a2) ? 1 : 0; pc += 4; break;
			case 8: program[a3] = (a1 == a2) ? 1 : 0; pc += 4; break;
			case 99: return 0;
			default:
				printf("Wrong opcode @ %d : %d\n", pc, program[pc]);
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
	char * s;
	char * token;
	int program[1204 * 32];
	int max_pos = 0;

	memset(program, 0, sizeof(program));
	s = fgets(buf, sizeof(buf), f);

	while ((token = strsep(&s, ",")) != NULL)
		program[max_pos++] = atoi(token);

	run(program);

	fclose(f);

	return 0;
}
