#include <stdint.h> // uintXX_t
#include <stdio.h>  // printf
#include <string.h> // memcpy

#include "jfif.h"

/**
 * This function constructs a JFIF Segment by parsing the given byte array.
 * 
 * @param to      The pointer to the first byte of the JFIF Segment
 * @param from    The pointer to the byte array to be parsed
 * @param seg_len The length of the JFIF Segment
 * 
 * @return a pointer to the first byte after the JFIF Segment
 */
uint8_t* jfif_construct_segment(struct JFIF_Segment *to, uint8_t *from, uint16_t seg_len) {
    uint8_t *offset = from;

    /* Obtain all the fields but RGBn and AMPF if exits */
    memcpy(to->Identifier, offset, 14);
    
    /* Swap bytes of the fields of the JFIF Segment which is in big-endian */
    jfif_byte_swap(to);
    offset += 14; /* skip the fields, now pointing at the RGBn field */

    /* Obtain the RGBn field */
    uint16_t n = to->XThumbnail * to->YThumbnail; /* [JFIF v1.02, p5] */
    if (n > 0) { /* it is possible the length of this field is 0 */
        to->RGBn = calloc(1, 3*n);
        offset += n; /* skip the RGBn field */
    }

    /* Obtain the AMPF field if exists */
    if (offset < from + seg_len) {
        memcpy(to->AMPF, offset, 4);
        offset += 4; /* skip the AMPF field */
    }

    jfif_print_info(to);

    return offset;
}

/**
 * Swap the bytes of the fields in JFIF Segment if there is a mismatch between the image and machine endianess.
 * 
 * @param segment The JFIF Segment whose fields need byte swap
 */
void jfif_byte_swap(struct JFIF_Segment *segment) {
    segment->Version  = __builtin_bswap16(segment->Version);
    segment->XDensity = __builtin_bswap16(segment->XDensity);
    segment->YDensity = __builtin_bswap16(segment->YDensity);
}

/**
 * This function prints information about the JFIF Segment.
 * 
 * @param segment The JFIF Segment whose information to be displayed
 */
void jfif_print_info(struct JFIF_Segment *segment) {
    printf("JFIF v%d.%d\n", segment->Version >> 8, segment->Version & 0xff);
    printf("Unit:                             %d\n", segment->Unit);
    printf("Horizontal pixel density:         %d\n", segment->XDensity);
    printf("Vertical pixel density:           %d\n", segment->YDensity);
    printf("Thumbnail horizontal pixel count: %d\n", segment->XThumbnail);
    printf("Thumbnail vertical pixel count:   %d\n\n", segment->YThumbnail);
}
