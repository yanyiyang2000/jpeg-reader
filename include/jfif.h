#include <stdint.h> // uintXX_t
#include <stdlib.h> // size_t

#ifndef JFIF_H
#define JFIF_H

/**
 * [JFIF v1.02, p.5]
 * 
 * JFIF Marker Segment
 * 
 * @note The byte order of the fields is big-endian.
 */
struct __attribute__((packed)) JFIF_Segment {
    uint8_t  Identifier[5]; /* NULL terminated ASCII string “JFIF\0” */
    uint16_t Version;       /* The most significant byte is used for major revisions, the least significant byte for minor revisions */
    uint8_t  Unit;          /* The units for the X and Y densities */
    uint16_t XDensity;      /* The horizontal pixel density */
    uint16_t YDensity;      /* The vertical pixel density */
    uint8_t  XThumbnail;    /* The thumbnail horizontal pixel count */
    uint8_t  YThumbnail;    /* The thumbnail vertical pixel count */
    uint8_t  *RGBn;         /* The packed (24-bit) RGB values for the thumbnail pixels, n = Xthumbnail * Ythumbnail */
    uint8_t  AMPF[4];       /* ASCII string "AMPF" indicating the existence of MPF images (Apple) */
};

/**
 * This function performs byte swap on the fields of JFIF Segment.
 * 
 * @param segment The pointer to the JFIF Segment
 */
void jfif_byte_swap(struct JFIF_Segment *segment);

/**
 * This function prints information of JFIF Segment.
 * 
 * @param segment The pointer to the JFIF Segment
 */
void jfif_print_info(struct JFIF_Segment *segment);

/**
 * This function constructs a JFIF Segment by parsing the given byte array.
 * 
 * @param seg     The pointer to the JFIF Segment
 * @param ptr     The pointer to the pointer to the byte array to be parsed
 * @param seg_len The length of the JFIF Segment
 */
void jfif_construct_segment(struct JFIF_Segment *seg, uint8_t **ptr, uint16_t seg_len);

#endif /* JFIF_H */
