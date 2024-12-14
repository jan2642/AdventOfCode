#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

static int num_rules = 0;

typedef struct {
    int a;
    int b;
} rule_t;

static rule_t rules[2000];

static int verify_rule(int a, int b)
{
    for (int i = 0; i < num_rules; i++) {
        if ((rules[i].a == a) && (rules[i].b == b))
            return 1; /* Valid match */
        if ((rules[i].a == b) && (rules[i].b == a))
            return -1; /* Violation */
    }

    return 0; /* No rule found */
}

static int compare(const void* a, const void* b) {
   return verify_rule(*(int*)a, *(int*)b);
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
    int total_a = 0;
    int total_b = 0;
    
    /* Read rules */
    while ((linelen = getline(&line, &linecap, fp)) > 0) {
        if (*line == '\n') /* end of rules */
            break;

        char *p = index(line, '|');
        *p = '\0';
        p++;

        rules[num_rules].a = atoi(line);
        rules[num_rules].b = atoi(p);
        num_rules++;
    }

    printf("Rules: %d\n", num_rules);

    /* pages */
    while ((linelen = getline(&line, &linecap, fp)) > 0) {
        int pages[100];
        int num_pages = 0;

        char *p = strdup(line);
        char *ap;
        while ((ap = strsep(&p, ",\n")))
            if (*ap != '\0')
                pages[num_pages++] = atoi(ap);
        free(p);

        int error = 0;
        for (int i = 0; i < num_pages - 1 && !error; i++)
            for (int j = i + 1; j < num_pages && !error; j++)
                if (verify_rule(pages[i], pages[j]) == -1)
                    error = 1;

        if (!error) {
            total_a += pages[num_pages / 2];
        }
        else {
            qsort(pages, num_pages, sizeof(int), compare);
            total_b += pages[num_pages / 2];
        }
    }

    printf("Total A: %d\n", total_a);
    printf("Total B: %d\n", total_b);

}
