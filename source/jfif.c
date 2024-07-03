#include <stdint.h> // uintXX_t
#include <stdio.h>  // printf
#include <string.h> // memcpy

#include "jfif.h"

uint8_t* jfif_construct_segment(struct JFIF_Segment *to, uint8_t *from, uint16_t seg_len) {
    uint8_t *ptr = from; // pointer to the current byte

    /* Obtain the JFIF Segment excluding RGBn and AMPF fields */
    memcpy(to->Identifier, ptr, 14);
    
    /* Swap bytes of the fields of the JFIF Segment which is in big-endian */
    jfif_byte_swap(to);

    /* Skip the fields, now pointing at the RGBn field */
    ptr += 14;

    /* Obtain the RGBn field if exists */
    uint16_t n = to->XThumbnail * to->YThumbnail;   // [JFIF v1.02, p.5]
    if (n > 0) {                                    // it is possible the length of this field is 0
        to->RGBn = calloc(1, 3*n);
        ptr += n;                                   // skip the RGBn field
    }

    /* Obtain the AMPF field if exists */
    if (ptr < from + seg_len) {
        memcpy(to->AMPF, ptr, 4);
        ptr += 4; // skip the AMPF field
    }

    jfif_print_info(to);

    return ptr;
}

void jfif_byte_swap(struct JFIF_Segment *segment) {
    segment->Version  = __builtin_bswap16(segment->Version);
    segment->XDensity = __builtin_bswap16(segment->XDensity);
    segment->YDensity = __builtin_bswap16(segment->YDensity);
}

void jfif_print_info(struct JFIF_Segment *segment) {
    printf("JFIF v%d.%d\n", segment->Version >> 8, segment->Version & 0xff);
    printf("Unit:                             %d\n", segment->Unit);
    printf("Horizontal pixel density:         %d\n", segment->XDensity);
    printf("Vertical pixel density:           %d\n", segment->YDensity);
    printf("Thumbnail horizontal pixel count: %d\n", segment->XThumbnail);
    printf("Thumbnail vertical pixel count:   %d\n\n", segment->YThumbnail);
}
