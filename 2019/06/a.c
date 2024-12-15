#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

typedef struct item item;

struct item {
	item * parent;
	item * next;
	char * name;
};

static item root = { NULL, NULL, "COM" };
static item *last = &root;

item * get_item(char * name) {
	item * i;

	for (item * i = &root; i; i = i->next)
		if (strcmp(name, i->name) == 0)
			return i;

	i = (item *)calloc(sizeof(item), 1);
	i->name = strdup(name);
	last->next = i;
	last = i;

	return i;
}

int main(int argc, char ** argv)
{
	FILE *f = fopen("input.txt", "r");

	char buf[32];
	char * s;
	char * e;
	char * t;

	while ((s = fgets(buf, sizeof(buf), f))) {
		*(index(s, '\n')) = '\0';
		e = index(s, ')');
		t = e + 1;
		*e = '\0';
	
		item * si = get_item(s);
		item * ti = get_item(t);
		ti->parent = si;
	}

	int c = 0;
	for (item * i = &root; i; i = i->next)
		for (item * j = i; j != &root; j = j->parent)
			c++;

	printf("%d\n", c);

	fclose(f);
}
