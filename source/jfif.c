#include <inttypes.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "jpeg.h"
#include "jfif.h"


void jfif_construct(struct JFIF_Segment *seg, uint8_t **ptr) {
    uint16_t seg_len   = 0;

    /* Skip Marker field, now pointing at Length field */
    *ptr += 2;
    seg->Base = *ptr;

    /* Parse Length field */
    seg_len = __builtin_bswap16(**(uint16_t **)ptr);

    /* Skip Length field, now pointing at Identifier field */
    *ptr += 2;

    /* Parse Identifier field */
    if (strncmp(*(const char **)ptr, "JFIF", 5) != 0) {
        printf("Unknown identifier\n");
        return;
    }

    /* Skip JFIF Segment, now pointing at the Marker field of the next Marker Segment */
    *ptr = seg->Base + seg_len;
}

void jfif_parse(struct JFIF_Segment *seg) {
    uint8_t *ptr = seg->Base;

    /* Skip Length field, now pointing at Identifier field */
    *ptr += 2;

    /* Skip Identifier field, now pointing at Version field */
    *ptr += 5;

    /* Parse Version Major field */
    printf("┌──────────────────────────────────────────┐\n");
    printf("│                   APP0                   │\n");
    printf("├──────────────────────────────────┬───────┤\n");
    printf("│ JFIF Major Version               │ %-5"PRIu8" │\n", *(uint8_t *)ptr);
    printf("├──────────────────────────────────┼───────┤\n");

    /* Skip Version Major field, now pointing at Version Minor field */
    ptr += 1;

    /* Parse Version Minor field */
    printf("│ JFIF Minor Version               │ %-5"PRIu8" │\n", *(uint8_t *)ptr);
    printf("├──────────────────────────────────┼───────┤\n");

    /* Skip Version Minor field, now pointing at Unit field */
    ptr += 1;

    /* Parse Unit field */
    printf("│ Unit                             │ %-5"PRIu8" │\n", *(uint8_t *)ptr);
    printf("├──────────────────────────────────┼───────┤\n");

    /* Skip Unit field, now pointing at Horizontal Pixel Density field */
    ptr += 1;

    /* Parse Horizontal Pixel Density field */
    printf("│ Horizontal Pixel Density         │ %-5"PRIu16" │\n", __builtin_bswap16(*(uint16_t *)ptr));
    printf("├──────────────────────────────────┼───────┤\n");

    /* Skip Horizontal Pixel Density field, now pointing at Vertical Pixel Density field */
    ptr += 2;

    /* Parse Vertical Pixel Density field */
    printf("│ Vertical Pixel Density           │ %-5"PRIu16" │\n", __builtin_bswap16(*(uint16_t *)ptr));
    printf("├──────────────────────────────────┼───────┤\n");

    /* Skip Vertical Pixel Density field, now pointing at Thumbnail Horizontal Pixel Count field */
    ptr += 2;

    /* Parse Thumbnail Horizontal Pixel Count field */
    printf("│ Thumbnail Horizontal Pixel Count │ %-5"PRIu8" │\n", *(uint8_t *)ptr);
    printf("├──────────────────────────────────┼───────┤\n");

    /* Skip Thumbnail Horizontal Pixel Count field, now pointing at Thumbnail Vertical Pixel Count field */
    ptr += 1;

    /* Parse Thumbnail Vertical Pixel Count field */
    printf("│ Thumbnail Vertical Pixel Count   │ %-5"PRIu8" │\n", *(uint8_t *)ptr);
    printf("└──────────────────────────────────┴───────┘\n\n");

    /* Skip Thumbnail Vertical Pixel Count field */
    ptr += 1;
}

void jfif_free(struct JFIF_Segment *seg) {
    /* Nothing to be freed */
}