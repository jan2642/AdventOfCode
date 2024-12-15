#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define W 25
#define H 6

int main(int argc, char ** argv)
{
	FILE *f = fopen("input.txt", "r");

	char buf[1024];
	char * s;
	int seq_prev[1024], seq[1024];
	int max;

	s = fgets(buf, sizeof(buf), f);
	*(index(s, '\n')) = '\0';
	for (max = 0; *s; s++, max++)	
		seq_prev[max] = *s - '0';
	fclose(f);

	int mul[] = { 0, 1, 0, -1 };

	for (int i = 0; i < 100; i++) {
		memset(seq, 0, sizeof(seq));

		for (int j = 0; j < max; j++) {
			for (int k = 0; k < max; k++) {
				seq[j] += (seq_prev[k] * mul[((k + 1) / (j + 1)) % 4]);
			}
			seq[j] = abs(seq[j] % 10);
		}
		
		memcpy(seq_prev, seq, sizeof(seq));
	}

	for (int j = 0; j < 8; j++)
		printf("%d", seq[j]);
	printf("\n");
	return 0;
}
