#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

typedef struct pattern_t pattern_t;

struct pattern_t {
    char *pattern;
    int len;
    pattern_t *next;
};

static pattern_t * phead = NULL, * ptail = NULL;

static void pattern_insert(char *pattern)
{
    pattern_t *item = calloc(sizeof(pattern_t), 1);
    item->pattern = strdup(pattern);
    item->len = strlen(pattern);
    item->next = phead;
    phead = item;
    if (!ptail)
        ptail = phead;
}

static uint64_t lookup(char *design, int pos, int64_t *matches) {
    int len = strlen(design);

    if (pos == len)
        return 1;

    if (matches[pos] < 0) {
        matches[pos] = 0;

        for (pattern_t * p = phead; p; p = p->next)
            if (memcmp(design + pos, p->pattern, p->len) == 0)
                matches[pos] += lookup(design, pos + p->len, matches);
    }
    return matches[pos];
}

int main(int argc, char **argv)
{
    if (argc != 2) {
        printf("%s <input.txt>\n", argv[0]);
        exit(1);
    }

    FILE *fp = fopen(argv[1], "r");
    char *buf = calloc(8 * 1024, 1);

    /* patterns */
    fscanf(fp, "%[a-z, ]\n\n", buf);
    char *ap, *p = buf;
    while ((ap = strsep(&p, ", \n")))
        if (*ap != '\0')
            pattern_insert(ap);

    /* designs */
    uint64_t total = 0;
    while (fscanf(fp, "%s\n", buf) == 1) {
        int64_t matches[strlen(buf)];
        memset(matches, 0xff, strlen(buf) * sizeof(int64_t));
        uint64_t r = lookup(buf, 0, matches);
        printf("%16llu: %s\n", r, buf);
        total += r;
    }
    printf("Total: %llu\n", total);

    free(buf);
}
