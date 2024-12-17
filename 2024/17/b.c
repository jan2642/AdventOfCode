#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>

typedef struct {
	uint64_t program[32];
	uint64_t reg[3];
	uint32_t pc;
	uint32_t max_pos;
    uint32_t out;
} state;

uint64_t combo(state * s, uint32_t pc)
{
    return s->program[pc] < 4 ? s->program[pc] : s->reg[s->program[pc] - 4];
}

int64_t state_run(state * s)
{
    do {
        switch (s->program[s->pc]) { /* Execute opcode */
            /* adv */  case 0: s->reg[0] = s->reg[0] / (1ULL << combo(s, s->pc + 1)); s->pc += 2; break;
            /* bxl */  case 1: s->reg[1] ^= s->program[s->pc + 1]; s->pc += 2; break;
            /* bst */  case 2: s->reg[1] = combo(s, s->pc + 1) % 8; s->pc += 2; break;
            /* jnz */  case 3: if (s->reg[0]) s->pc = s->program[s->pc + 1]; else s->pc += 2; break;
            /* bxc */  case 4: s->reg[1] ^= s->reg[2]; s->pc += 2; break;
            /* out */  case 5: s->out = combo(s, s->pc + 1) % 8; s->pc += 2; return 1;
            /* bdv */  case 6: s->reg[1] = s->reg[0] / (1ULL << combo(s, s->pc + 1)); s->pc += 2; break;
            /* cdv */  case 7: s->reg[2] = s->reg[0] / (1ULL << combo(s, s->pc + 1)); s->pc += 2; break;
        }

    }
    while (s->pc < s->max_pos);

    return 0;
}

int run(state *st, uint64_t a, uint32_t *out)
{
    state tmp;
    int r, i = 0;
    
    memcpy(&tmp, st, sizeof(tmp));
    tmp.pc = 0;
    tmp.reg[0] = a;

    do {
        if ((r = state_run(&tmp)))
            out[i++] = tmp.out;
    }
    while (r);

    return i; /* The amount of output values */
}

uint64_t search(state *st, uint64_t a, int n)
{
    uint32_t out[st->max_pos + 1];
    uint64_t result = 0;

    if (n > st->max_pos)
        return a;

    a <<= 3;

    for (int i = 0; i < 8; i++) {
        int r = run(st, a + i, out);

        /* Compare the output with the last n program values */
        int mismatch = 0;
        for (int j = 0; (j < r) && (mismatch == 0); j++)
            if (out[j] != st->program[st->max_pos - n + j])
                mismatch = 1;

        if (mismatch == 0) /* Match! Go deeper */
            if ((result = search(st, a + i, n + 1)))
                return result;
    }

    return 0;
}

int main(int argc, char ** argv)
{
    if (argc != 2) {
        printf("%s <input.txt>\n", argv[0]);
        exit(1);
    }

    FILE *fp = fopen(argv[1], "r");
	
    state * st = calloc(1, sizeof(state));
	char buf[1024 * 32];

    fscanf(fp, "Register A: %llu\n", &st->reg[0]);
    fscanf(fp, "Register B: %llu\n", &st->reg[1]);
    fscanf(fp, "Register C: %llu\n", &st->reg[2]);
    fscanf(fp, "\n");
    fscanf(fp, "Program: %s\n", buf);
    fclose(fp);

	char * token, * s = buf;
	int max_pos = 0;
	while ((token = strsep(&s, ",")) != NULL)
		st->program[max_pos++] = strtoull(token, NULL, 0);
	st->max_pos = max_pos;

    printf("%llu\n", search(st, 0, 1));

    return 0;
}
