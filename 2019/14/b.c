#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

typedef struct chem chem;
typedef struct recipe recipe;

struct recipe {
	int64_t amount;
	chem * chem;
	recipe * next;
};

struct chem {
	char * name;
	int64_t amount;
	int64_t available;
	recipe * recipe;
	chem * next;
};

static chem * clist = NULL;

chem * find_chem(char * name)
{
	for (chem * c = clist; c; c = c ->next)
		if (strcmp(name, c->name) == 0)
			return c;
	return NULL;
}

chem * add_chem(char * name, int64_t amount)
{
	chem * c = find_chem(name);
	if (c) {
		if (c->amount != -1) {
			printf("%s has already an amount: %lld\n", name, c->amount);
			exit(1);
		}
		c->amount = amount;
		return c;
	}
	
	c = calloc(1, sizeof(chem));
	c->name = strdup(name);
	c->amount = amount;

	c->next = clist;
	clist = c;

	return c;	
}

void add_recipe(chem * c, char * name, int64_t amount)
{
	chem * r = find_chem(name);
	if (!r) r = add_chem(name, -1);
	recipe * rec = calloc(1, sizeof(recipe));
	rec->chem = r;
	rec->amount = amount;
	rec->next = c->recipe;
	c->recipe = rec;
}

int64_t produce(chem * c, int64_t amount)
{
	int64_t ore = 0;
	if (!c->recipe) /* This is ORE */
		return amount;
	amount -= c->available;
	if (amount > 0) {
		for (recipe * r = c->recipe; r; r = r->next) {
			ore += produce(r->chem, r->amount * ((amount + c->amount - 1) / c->amount));
		}
		amount -= c->amount * ((amount + c->amount - 1) / c->amount);
	}
	c->available = -amount;

	return ore;
}

int main(int argc, char ** argv)
{
	FILE *f = fopen("input.txt", "r");
	char buf[1024];
	char * s, * token;
	while((s = fgets(buf, sizeof(buf), f))) {
		char * p = index(s, '=');
		*(p - 1) = '\0'; p += 2;
		int amount;
		char name[128];
		sscanf(p, "%d %s\n", &amount, name);
		chem * c = add_chem(name, amount);
		while ((token = strsep(&s, ",")) != NULL) {
			while (*token == ' ') token++;
			sscanf(token, "%d %s", &amount, name);
			add_recipe(c, name, amount);
		}
	}
	fclose(f);

	uint64_t ore = 1000000000000; /* Available ore */
	uint64_t f_min = 1, f_max = 1, f_target;
	chem * fuel = find_chem("FUEL");

	while (1) { /* Find initial bounds */
		for (chem * c = clist; c; c = c->next) c->available = 0; /* Start fresh */
		int64_t o = produce(fuel, f_max);
		if (o > ore) break; /* Overshoot, f_max found */
		f_min = f_max;
		f_max *= 2;
	}

	while(f_max - f_min > 1) { /* Binary search */
		for (chem * c = clist; c; c = c->next) c->available = 0; /* Start fresdh */
		f_target = (f_min + f_max) / 2;
		int64_t o = produce(fuel, f_target);
		if (o < ore) f_min = f_target; else f_max = f_target;
	}
	printf("Max fuel with %lld ore: %lld\n", ore, f_target);
}
