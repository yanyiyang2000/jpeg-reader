#include <inttypes.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "jpeg.h"
#include "jfif.h"


void jfif_construct(struct JFIF_Segment *seg, uint8_t **ptr) {
    uint8_t  *seg_base = NULL;
    uint16_t seg_len  = 0;

    /* Skip MARKER, now pointing at LENGTH */
    *ptr += 2;
    seg_base = *ptr;
    
    /* Parse LENGTH */
    seg_len = __builtin_bswap16(**(uint16_t **)ptr);

    /* Skip LENGTH, now pointing at IDENTIFIER */
    *ptr += 2;

    /* Parse IDENTIFIER */
    if (strncmp(*(const char **)ptr, "JFIF", 5) != 0) {
        printf("Unknown identifier\n");
        return;
    }

    /* Skip IDENTIFIER, now pointing at VERSION MAJOR */
    *ptr += 5;
    seg->Base = *ptr;

    /* Skip JFIF Segment, now pointing at MARKER of the next Marker Segment */
    *ptr = seg_base + seg_len;
}

void jfif_parse(struct JFIF_Segment *seg) {
    uint8_t *ptr = seg->Base;

    /* Parse and skip VERSION MAJOR */
    printf("┌──────────────────────────────────────────┐\n");
    printf("│                   APP0                   │\n");
    printf("├──────────────────────────────────┬───────┤\n");
    printf("│ JFIF Major Version               │ %-5"PRIu8" │\n", *(uint8_t *)ptr);
    printf("├──────────────────────────────────┼───────┤\n");
    ptr += 1;

    /* Parse and skip VERSION MINOR */
    printf("│ JFIF Minor Version               │ %-5"PRIu8" │\n", *(uint8_t *)ptr);
    printf("├──────────────────────────────────┼───────┤\n");
    ptr += 1;

    /* Parse and skip UNIT */
    printf("│ Unit                             │ %-5"PRIu8" │\n", *(uint8_t *)ptr);
    printf("├──────────────────────────────────┼───────┤\n");
    ptr += 1;

    /* Parse and skip HORIZONTAL PIXEL DENSITY */
    printf("│ Horizontal Pixel Density         │ %-5"PRIu16" │\n", __builtin_bswap16(*(uint16_t *)ptr));
    printf("├──────────────────────────────────┼───────┤\n");
    ptr += 2;

    /* Parse and skip VERTICAL PIXEL DENSITY */
    printf("│ Vertical Pixel Density           │ %-5"PRIu16" │\n", __builtin_bswap16(*(uint16_t *)ptr));
    printf("├──────────────────────────────────┼───────┤\n");
    ptr += 2;

    /* Parse and skip THUMBNAIL HORIZONTAL PIXEL COUNT */
    printf("│ Thumbnail Horizontal Pixel Count │ %-5"PRIu8" │\n", *(uint8_t *)ptr);
    printf("├──────────────────────────────────┼───────┤\n");
    ptr += 1;

    /* Parse and skip THUMBNAIL VERTICAL PIXEL COUNT */
    printf("│ Thumbnail Vertical Pixel Count   │ %-5"PRIu8" │\n", *(uint8_t *)ptr);
    printf("└──────────────────────────────────┴───────┘\n\n");
    ptr += 1;
}

void jfif_free(struct JFIF_Segment *seg) {
    /* Nothing to be freed */
}