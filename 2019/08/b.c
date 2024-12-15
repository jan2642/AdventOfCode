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

	char result[W * H];

	memset(result, '2', sizeof(result));

	while (*s) {
		for (int i = 0 ; i < W * H; i++)
			if (s[i] != '2' && result[i] == '2')
				result[i] = s[i];
		s += W * H;
	}

	for (int y = 0; y < H; y++) {
		for (int x = 0; x < W; x++)
			printf("%s", result[y * W + x] == '1' ? "##" : "  ");
		printf("\n");
	}

	fclose(f);
}
