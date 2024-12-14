#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int is_safe(char *line, int *n, int skip)
{
    char *ap;
    int prev = -1;
    int updown = 0;
    int unsafe = 0;
    char *copy = strdup(line);
    char *l = copy;
    printf("%s", l);
    int c = 0;
    while ((ap = strsep(&l, " \t\n"))) {
        if (*ap) {
            if (c++ == skip)
                continue;

            int current = strtol(ap, NULL, 0);
            if (prev == -1) {
                prev = current;
                continue;
            }

            if (abs(prev - current) > 3)
                unsafe = 1;

            if (abs(prev - current) < 1)
                unsafe = 1;

            if (updown == 0) {
                updown = prev < current ? 1 : -1;
            }
            else if ((updown == -1) && prev < current)
                unsafe = 1;
            else if ((updown == 1) && prev > current)
                unsafe = 1;

            prev = current;
        }
    }
    free(copy);

    if (n) *n = c;

    return unsafe == 0;
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
    
    int safe = 0;
    while ((linelen = getline(&line, &linecap, fp)) > 0) {
        int n;

        /* First test the original line */
        if (is_safe(line, &n, -1))
            safe++;
        /* if that fails, start skipping elements */
        else {
            printf("n = %d\n", n);
            for (int i = 0; i < n; i++) {
                if (is_safe(line, NULL, i)) {
                    safe++;
                    break;
                }
            }
        }
    }

    printf("safe: %d\n", safe);
}
