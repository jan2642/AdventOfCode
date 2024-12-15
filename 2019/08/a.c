#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define W 25
#define H 6

int main(int argc, char ** argv)
{
	FILE *f = fopen("input.txt", "r");

	char buf[1024 * 32];
	char * s;

	s = fgets(buf, sizeof(buf), f);
	*(index(s, '\n')) = '\0';

	int min_0 = 99999999;
	int mul = 0;

	while (*s) {
		char * l = strndup(s, W * H);
		int count_0 = 0, count_1 = 0, count_2 = 0;
		for (char * p = l; *p; p++) {
			switch (*p) {
				case '0': count_0++; break;
				case '1': count_1++; break;
				case '2': count_2++; break;
			}
		}

		if (count_0 < min_0) {
			min_0 = count_0;
			mul = count_1 * count_2;
		}

		free(l);
		s += W * H;
	}

	printf("==> %d %d\n", min_0, mul);

	fclose(f);
}
