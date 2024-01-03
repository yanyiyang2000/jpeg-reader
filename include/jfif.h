#include <stdint.h> // uintXX_t
#include <stdlib.h> // size_t

#ifndef JFIF_H
#define JFIF_H

/**
 * [JFIF v1.02, p5]
 * JFIF Marker Segment
 * 
 * @note The byte order of the fields is big-endian.
 */
typedef struct __attribute__((packed)) {
    uint8_t  Identifier[5]; /* NULL terminated ASCII string “JFIF\0” */
    uint16_t Version;       /* The most significant byte is used for major revisions, the least significant byte for minor revisions */
    uint8_t  Unit;          /* Units for the X and Y densities */
    uint16_t XDensity;      /* Horizontal pixel density */
    uint16_t YDensity;      /* Vertical pixel density */
    uint8_t  XThumbnail;    /* Thumbnail horizontal pixel count */
    uint8_t  YThumbnail;    /* Thumbnail vertical pixel count */
    uint8_t  *RGBn;         /* Packed (24-bit) RGB values for the thumbnail pixels, n = Xthumbnail * Ythumbnail */
    uint8_t  AMPF[4];       /* ASCII string "AMPF" indicating the existence of MPF images (Apple) */
} JFIF_Segment;

void jfif_byte_swap(JFIF_Segment *segment);

void jfif_print_info(JFIF_Segment *segment);

uint8_t* jfif_construct_segment(JFIF_Segment *to, uint8_t *from, uint16_t seg_len);

#endif /* JFIF_H */
