#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int valid_pw(int pw)
{
	char buf[16];

	snprintf(buf, sizeof(buf), "%d", pw);

	int d = buf[0];
	int dd = 0;
	for (int i = 1; i < strlen(buf); i++) {
		if (buf[i] == d)
			dd = 1;
		else if (buf[i] < d)
			return 0;
		d = buf[i];
	}

	return dd;
}

int main(int argc, char ** argv)
{
	int total = 0;

	for (int i = 272091; i <= 815432; i++)
		total += valid_pw(i);

	printf("%d\n", total);
}
