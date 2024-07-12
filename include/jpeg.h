#include <stdint.h> // uintXX_t

#include "exif.h"
#include "jfif.h"

#ifndef JPEG_H
#define JPEG_H

/**
 * [ISO/IEC 10918-1 Annex B Compressed data formats]
 * 
 * B.1.1.4 Marker segments
 * 
 * A marker segment consists of a marker followed by a sequence of related parameters. The first parameter in a marker
 * segment is the two-byte length parameter. This length parameter encodes the number of bytes in the marker segment,
 * including the length parameter and excluding the two-byte marker.
 */
struct __attribute__((packed)) Marker_Segment {
    uint8_t  Marker[2]; /* The marker of the segment */
    uint16_t Length;    /* The length of the segment (always in big-endian) */
    union {             /* The pointer to the segment (JFIF or Exif) */
        struct JFIF_Segment *jfif_segment;
        struct Exif_Segment *exif_segment;
    };
};

/**
 * This function constructs a Marker Segment by parsing the given byte array.
 * 
 * @param to      The pointer to the first byte of the Marker Segment
 * @param from    The pointer to the byte array to be parsed
 * @param seg_len The length of the Marker Segment
 * 
 * @return A pointer to the first byte after the Marker Segment
 */
uint8_t* construct_marker_segment(struct Marker_Segment *to, uint8_t *from, uint16_t seg_len);

#endif /* JPEG_H */
