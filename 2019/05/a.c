#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int shiftdec(int a, int n)
{
	while (n--)
		a /= 10;
	return a;
}

int run(int * program)
{
	int pc = 0;
	char buf[32];
	char * s;

	do {
		int a1 = (shiftdec(program[pc], 2) % 10) ? program[pc + 1] : program[program[pc + 1]];
		int a2 = (shiftdec(program[pc], 3) % 10) ? program[pc + 2] : program[program[pc + 2]];

		switch(program[pc] % 100) {
			case 3:
				printf("Input: ");
				s = fgets(buf, sizeof(buf), stdin);
				program[program[pc + 1]] = atoi(s);
				pc += 2;
				break;
			case 4:
				printf("Output: %d\n", program[program[pc + 1]]);
				pc += 2;
				break;
			case 1:
				program[program[pc + 3]] = a1 + a2;
				pc += 4;
				break;
			case 2:
				program[program[pc + 3]] = a1 * a2;
				pc += 4;
				break;
			case 99:
				break;
			default:
				printf("Wrong opcode: %d\n", program[pc]);
				return -1;
		}
	}
	while(program[pc] != 99);

	return 0;
}

int main(int argc, char ** argv)
{
	FILE *f = fopen("input.txt", "r");

	char buf[1024];
	char * s;
	char * token;
	int program[1204];
	int max_pos = 0;

	s = fgets(buf, sizeof(buf), f);

	while ((token = strsep(&s, ",")) != NULL)
		program[max_pos++] = atoi(token);

	run(program);

	fclose(f);

	return 0;
}
