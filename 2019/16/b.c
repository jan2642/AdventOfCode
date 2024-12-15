#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char ** argv)
{
	FILE *f = fopen("input.txt", "r");

	char buf[1024];
	char * s;
	int * seq = malloc(650 * 10000 * sizeof(int));
	int max;

	s = fgets(buf, sizeof(buf), f);
	*(index(s, '\n')) = '\0';
	for (max = 0; *s; s++, max++)
		seq[max] = *s - '0';
	fclose(f);

	for (int i = 0; i < 10000 - 1; i++)
		memcpy(&seq[max * (i + 1)], &seq[max * i], max * sizeof(*seq));

	int offset = 0;
	for (int i = 0; i < 7; i++)
		offset = offset * 10 + seq[i];

	max *= 10000;
	printf("offset: %d / %d\n", offset, max);
	
	for (int i = 0; i < 100; i++) {
		int cur = seq[max - 1];
		for (int j = max - 2; j >= offset; j--) {
			cur += seq[j];
			cur %= 10;
			seq[j] = cur;
		}
	}
	for (int j = offset; j < offset + 8; j++)
		printf("%d", seq[j]);
	printf("\n");
}
