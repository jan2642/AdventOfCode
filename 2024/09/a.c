#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>

/* Find the offset of 'val' in buf */
int32_t buf_index(int32_t *buf, int32_t val)
{
    int32_t offs = 0;
    while (*buf != -2) {
        if (*buf == val)
            return offs;
        buf++;
        offs++;
    }

    return -1;
}

int main(int argc, char ** argv)
{
    if (argc != 2) {
        printf("%s <input.txt>\n", argv[0]);
        exit(1);
    }

    FILE *fp = fopen(argv[1], "r");
	int32_t *buf = calloc(1024 * 1024 * 8, 1);
    int p = 0;
    int id = 0;
    int filled = 0;

	while (!feof(fp)) {
		char c = fgetc(fp);
        if (feof(fp))
            break;

        int n = c - '0';
        for (int i = 0; i < n; i++)
            buf[p++] = id;
        

        filled += n;
        id++;

		c = fgetc(fp);
        if (feof(fp))
            break;

        n = c - '0';
        for (int i = 0; i < n; i++)
            buf[p++] = -1;
	}
	fclose(fp);

    buf[p] = -2; /* End-of-buffer */
            
    printf("total: %d   data: %d   max_id: %d\n", p, filled, id);

    /* Compress */
    int len = p;
    int32_t offs;
    while ((offs = buf_index(buf, -1)) < filled) {
        int last = len - 1;
        while (buf[last] == -1) last--;
        buf[offs] = buf[last];
        buf[last] = - 1;
    }

    uint64_t crc = 0;
    
    for (int i = 0; i < filled; i++)
        crc += i * (buf[i]);

    printf("crc: %llu\n", crc);

	return 0;
}
