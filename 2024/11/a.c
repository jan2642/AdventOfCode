#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define HT_SIZE (4096)

/* Extremely rudimentary hashtable */
static void ht_insert(int64_t **table, int64_t key, int64_t value)
{
    int pos = key % HT_SIZE;
    int i;

    for (i = 0; (i < HT_SIZE) && (table[0][(pos + i) % HT_SIZE] != -1); i++);
    table[0][(pos + i) % HT_SIZE] = key;
    table[1][(pos + i) % HT_SIZE] = value;
}

static void ht_add(int64_t **table, int64_t key, int64_t value)
{
    int pos = key % HT_SIZE;
    int i;

    for (i = 0; (i < HT_SIZE) && (table[0][(pos + i) % HT_SIZE] != -1); i++)
        if (table[0][(pos + i) % HT_SIZE] == key) {
            table[1][(pos + i) % HT_SIZE] += value;
            return;
        }

    /* If we got here, it doesn't exist yet. i points to the next blank spot */
    table[0][(pos + i) % HT_SIZE] = key;
    table[1][(pos + i) % HT_SIZE] = value;
}

static void ht_clear(int64_t **table) {
    memset(table[0], 0xff, sizeof(int64_t) * HT_SIZE);
    memset(table[1], 0x00, sizeof(int64_t) * HT_SIZE);
}

static int64_t **ht_alloc()
{
    int64_t **table = calloc(sizeof(int64_t *), 2);
    table[0] = calloc(sizeof(int64_t), HT_SIZE);
    table[1] = calloc(sizeof(int64_t), HT_SIZE);
    ht_clear(table);

    return table;
}

static uint64_t digits(uint64_t a)
{
    int n = 1;
    while (a >= 10) {
        a /= 10;
        n++;
    }

    return n;
}

static uint64_t tens(uint64_t a)
{
    uint64_t n = 10;
    while (--a)
        n *= 10;
    return n;
}

int main(int argc, char **argv)
{
    if (argc != 2) {
        printf("%s <input.txt>\n", argv[0]);
        exit(1);
    }

    FILE *fp = fopen(argv[1], "r");
    char *line = NULL;
    size_t linecap = 0;
    ssize_t linelen;

    int steps = 25;

    int64_t **table = ht_alloc();
    
    while ((linelen = getline(&line, &linecap, fp)) > 0) {
        char *p = strdup(line);
        char *ap;
        while ((ap = strsep(&p, " \n")))
            if (*ap != '\0') {
                int64_t n = strtoll(ap, NULL, 0);

                ht_add(table, n, 1);
                printf("%llu\n", n);
            }
        free(p);
    }

    int64_t **next = ht_alloc();
    for (int i = 0; i < steps; i++) {
        for (int j = 0; j < HT_SIZE; j++) {
            int d = 0;
            if (table[0][j] == 0) {
                ht_add(next, 1, table[1][j]);
            }
            else if ((d = digits(table[0][j])) & 0x1)
                ht_add(next, table[0][j] * 2024, table[1][j]);
            else {
                int t = tens(d / 2);
                ht_add(next, table[0][j] % t, table[1][j]);
                ht_add(next, table[0][j] / t, table[1][j]);
            }
        }

        int64_t **old = table;
        table = next;
        next = old;
        ht_clear(next);
    }

    /* Sum all the items */
    uint64_t total = 0;
    for (int j = 0; j < HT_SIZE; j++) {
        total += table[1][j];
    }

    printf("Total: %llu\n", total);

}
