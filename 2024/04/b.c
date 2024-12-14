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

    char **lines = calloc(1000, sizeof(char *));
    int total = 0;
    int n = 0;
    while ((linelen = getline(&line, &linecap, fp)) > 0) {
        lines[n++] = strdup(line);
    }

    char *xmas = "XMAS";
    int w = strlen(lines[0]) - 1;

    printf("%d X %d\n", w, n);

    /* Diagonal */
    for (int l = 1; l < n - 1; l++) {
        for (int c = 1; c < w - 1; c++) {
            if (lines[l][c] != 'A')
                continue;

            if (!((lines[l - 1][c - 1] == 'M' && lines[l + 1][c + 1] == 'S') || 
                (lines[l - 1][c - 1] == 'S' && lines[l + 1][c + 1] == 'M')))
               continue; 
            if (!((lines[l - 1][c + 1] == 'M' && lines[l + 1][c - 1] == 'S') || 
                (lines[l - 1][c + 1] == 'S' && lines[l + 1][c - 1] == 'M')))
               continue;

           total++; 
        }
    }

    printf("total: %d\n", total);
    
}
