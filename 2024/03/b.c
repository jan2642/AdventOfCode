#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

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

    int total = 0;
    int enable = 1;
    while ((linelen = getline(&line, &linecap, fp)) > 0) {
        char *p = line;
        char *e, *c, *t;

        while (*p) {
            if (strncmp(p, "do()", 4) == 0) {
                enable = 1;
            }
            else if (strncmp(p, "don't()", 7) == 0) {
                enable = 0;
            }
            else if (enable && strncmp(p, "mul(", 4) == 0) {
                p += 4; /* Skip 'mul(' */
                c = index(p, ',');
                if (!c)
                    break;
                e = index(c, ')');
                if (!e)
                    break;

                int error = 0;

                /* Verify if there are only digits between p and the comma */
                for (t = p; t < c && error == 0; t++)
                    if ((*t < '0') || (*t > '9'))
                        error = 1;

                /* Verify if there are only digits between the comma and e*/
                for (t = c + 1; t < e && error == 0; t++)
                    if ((*t < '0') || (*t > '9'))
                        error = 1;

                if (error)
                    continue;

                int a = atoi(p);
                int b = atoi(c + 1);

                total += a * b;

                printf("%d %d\n", a, b);

                continue;
            }
            p++;
        }
    }

    printf("total: %d\n", total);
}
