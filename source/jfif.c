#include <stdint.h> // uintXX_t
#include <stdio.h>  // printf
#include <stdlib.h> // free
#include <string.h> // memcpy

#include "jfif.h"


void jfif_byte_swap(struct JFIF_Segment *segment) {
    segment->Version  = __builtin_bswap16(segment->Version);
    segment->XDensity = __builtin_bswap16(segment->XDensity);
    segment->YDensity = __builtin_bswap16(segment->YDensity);
}

void jfif_print_info(struct JFIF_Segment *segment) {
    printf("┌──────────────────────────────────────────┐\n");
    printf("│                   APP0                   │\n");
    printf("├──────────────────────────────────┬───────┤\n");
    printf("│ JFIF major version               │ %-5d │\n", segment->Version >> 8);
    printf("├──────────────────────────────────┼───────┤\n");
    printf("│ JFIF minor version               │ %-5d │\n", segment->Version & 0xff);
    printf("├──────────────────────────────────┼───────┤\n");
    printf("│ Unit                             │ %-5d │\n", segment->Unit);
    printf("├──────────────────────────────────┼───────┤\n");
    printf("│ Horizontal pixel density         │ %-5d │\n", segment->XDensity);
    printf("├──────────────────────────────────┼───────┤\n");
    printf("│ Vertical pixel density           │ %-5d │\n", segment->YDensity);
    printf("├──────────────────────────────────┼───────┤\n");
    printf("│ Thumbnail horizontal pixel count │ %-5d │\n", segment->XThumbnail);
    printf("├──────────────────────────────────┼───────┤\n");
    printf("│ Thumbnail vertical pixel count   │ %-5d │\n", segment->YThumbnail);
    printf("└──────────────────────────────────┴───────┘\n\n");
}

void jfif_construct_segment(struct JFIF_Segment *seg, uint8_t **ptr, uint16_t seg_len) {
    uint16_t n = 0;    // RGBn field length

    /* Obtain the JFIF Segment excluding RGBn and AMPF fields */
    memcpy(seg->Identifier, *ptr, 14);
    
    /* Swap bytes of the fields of the JFIF Segment which is in big-endian */
    jfif_byte_swap(seg);

    /* Skip the fields, now pointing at the RGBn field */
    *ptr += 14;

    /* Obtain the RGBn field if exists */
    n = seg->XThumbnail * seg->YThumbnail;  // [JFIF v1.02, p.5]
    if (n > 0) {                            // it is possible the length of this field is 0
        seg->RGBn = calloc(1, 3*n);
        *ptr += n;                          // skip the RGBn field
    }

    /* Obtain the AMPF field if exists */
    if (*ptr < *ptr + seg_len) {
        memcpy(seg->AMPF, *ptr, 4);
        *ptr += 4; // skip the AMPF field
    }

    jfif_print_info(seg);
}

void jfif_free_segment(struct JFIF_Segment *seg) {
    free(seg);
}
