#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define LISTS (2)
#define ROWS (2000)

int compare(const void* a, const void* b) {
   return (*(int*)a - *(int*)b);
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
    
    int **lists = (int **)calloc(LISTS, sizeof(int *));
    for (int i = 0; i < LISTS; i++)
        lists[i] = (int *)calloc(ROWS, sizeof(int));

    int n = 0;
    while ((linelen = getline(&line, &linecap, fp)) > 0) {
        char *ap;
        int list = 0;
        while ((ap = strsep(&line, " \t\n"))) {
            if (*ap) {
                lists[list++][n] = strtol(ap, NULL, 0);
            }
        }
        n++;
        if (list > LISTS) {
            printf("error: too many lists!");
            exit(1);
        }
        if (n > ROWS) {
            printf("error: too many rows!");
            exit(1);
        }
    }
    free(line);

    for (int l = 0; l < LISTS; l++)
        qsort(lists[l], n, sizeof(int), compare);

    int total = 0;
    for (int i = 0; i < n; i++) {
        printf("%d: %d - %d --> %d\n", i, lists[0][i], lists[1][i], abs(lists[0][i] - lists[1][i]));
        total += abs(lists[0][i] - lists[1][i]);
    }

    printf("total: %d\n", total);
}
