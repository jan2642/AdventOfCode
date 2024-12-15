#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char ** argv)
{
	FILE *f = fopen("input.txt", "r");

	int total = 0;
	char buf[16];
	char * s;

	while ((s = fgets(buf, sizeof(buf), f))) {
		int mass = atoi(s);
		for (int fuel = (mass / 3) - 2;  fuel > 0; fuel = (fuel / 3 - 2))
			total += fuel;
	}

	printf("total: %d\n", total);

	fclose(f);
}
