#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>

typedef struct item_t item_t;

struct item_t {
    int id;
    int len;
    item_t * next;
    item_t * prev;
};

/* Find the item with a specific id */
item_t *find_item(item_t *list, int id)
{
    item_t *item = list;
    while (item) {
        if (item->id == id)
            return item;
        item = item->next;
    }
    return NULL;
}

item_t *alloc_item(int id, int len, item_t *prev, item_t *next)
{
    item_t *item = calloc(sizeof(item_t), 1);
    item->id = id;
    item->len = len;
    item->prev = prev;
    if (prev)
        prev->next = item;
    item->next = next;
    if (next)
        next->prev = item;

    return item;
}

int main(int argc, char ** argv)
{
    if (argc != 2) {
        printf("%s <input.txt>\n", argv[0]);
        exit(1);
    }

    FILE *fp = fopen(argv[1], "r");
    item_t *disk = NULL;
    item_t *last = NULL;
    int p = 0;
    int id = 0;
    int filled = 0;

	while (1) {
		char c = fgetc(fp);
        if (feof(fp))
            break;

        int n = c - '0';
        if (n > 0) {
            last = alloc_item(id++, n, last, NULL);

            if (!disk)
                disk = last;
        }

		c = fgetc(fp);
        if (feof(fp))
            break;

        n = c - '0';
        if (n > 0)
            last = alloc_item(-1, n, last, NULL);
	}
	fclose(fp);

    /* Move files */
    for (int i = id; i >= 0; i--) {
        item_t *item = find_item(disk, i);
        if (!item)
            continue;

        /* Go through the empty space */
        item_t *iter = disk;
        while (iter) {
            if (iter == item) /* Don't move further, only to the front */
                break;

            if ((iter->id != -1) ||        /* Not empty space */
                (iter->len < item->len)) { /* Not large enough */
                iter = iter->next;
                continue;
            }

            /* Cut it from it's current location and replace it with empty space */
            item_t *empty = alloc_item(-1, item->len, item->prev, item->next);

            /* Merge potential empty spaces */

            if (empty && empty->prev && empty->prev->id == -1) { /* Prev is empty space */
                empty->prev->len += empty->len;
                if (empty->prev) empty->prev->next = empty->next;
                if (empty->next) empty->next->prev = empty->prev;
                item_t *old_empty = empty;
                empty = empty->prev;
                free(old_empty);
            }

            if (empty && empty->next && empty->next->id == -1) { /* Next is empty space */
                empty->next->len += empty->len;
                if (empty->next) empty->next->prev = empty->prev;
                if (empty->prev) empty->prev->next = empty->next;
                if (empty != iter) /* In some fringe case, this can happen. Avoid the double free */.
                    free(empty);
            }

            /* Insert it at it's new location */
            item->prev = iter->prev;
            if (iter->prev) iter->prev->next = item;

            if (iter->len == item->len) { /* Exact fit ! */
                item->next = iter->next;
                if (iter->next) iter->next->prev = item;
            }
            else /* Need a litle bit of empty filler to match the size */
                alloc_item(-1, iter->len - item->len, item, iter->next);

            free(iter);

            break;
        }
    }

    uint64_t crc = 0;
    uint64_t offs = 0;

    item_t *iter = disk;
    while (iter) {
        if (iter->id != -1)
            for (int i = 0; i < iter->len; i++)
                crc += (offs + i) * iter->id;
        offs += iter->len;
        iter = iter->next;
    }

    printf("crc: %llu\n", crc);
	return 0;
}
