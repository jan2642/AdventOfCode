#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int run(int * program, int a, int b)
{
	int pc = 0;

	program[1] = a;
	program[2] = b;

	do {
		switch(program[pc]) {
			case 1:
				program[program[pc + 3]] = program[program[pc + 1]] + program[program[pc + 2]];
				pc += 4;
				break;
			case 2:
				program[program[pc + 3]] = program[program[pc + 1]] * program[program[pc + 2]];
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

	return program[0];
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

	printf("%d\n", run(program, 12, 2));

	fclose(f);
}
