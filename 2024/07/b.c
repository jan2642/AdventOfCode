#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

uint64_t tens(uint64_t a)
{
    uint64_t n = 10;
    while (a >= n)
        n *= 10;
    return n;
}

uint64_t calculate(uint64_t a, uint64_t b, int op)
{
    switch(op) {
        case 0: a += b; break;              /* add */
        case 1: a *= b; break;              /* multiply */
        case 2: a = a * tens(b) + b; break; /* concatenate */
    }

    return a;
}

uint64_t evaluate(uint64_t expected, uint64_t *numbers, int n, uint64_t a)
{
    int result = 0;

    if (n == 0) /* Done! */
        return expected == a;

    if (a > expected) /* No point in going on... */
        return 0;

    for (int op = 0; op < 3 && !result; op++)
        result |= evaluate(expected, numbers + 1, n - 1, calculate(a, numbers[0], op));

    return result;
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
    uint64_t total = 0;
    
    while ((linelen = getline(&line, &linecap, fp)) > 0) {

        char *l = strdup(line);
        char *p = index(l, ':');
        *p = '\0';
        p++;

        uint64_t result = strtoll(l, NULL, 10);

        char *ap;
        int n = 0;
        uint64_t numbers[100];
        while ((ap = strsep(&p, " \t\n")))
            if (*ap)
                numbers[n++] = strtol(ap, NULL, 0);

        if (evaluate(result, numbers + 1, n - 1, numbers[0])) {
            printf("%llu\n", result);
            total += result;
        }

        free(l);
    }

    printf("Total: %llu\n", total);
}
