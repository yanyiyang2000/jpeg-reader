#include <stdio.h>   // fXXXX
#include <stdint.h>  // uint8_t
#include <stdlib.h>  // calloc
#include <string.h>  // memcpy

#include "jpeg.h"

/**
 * This helper function parses the Length field of Marker Segment.
 * 
 * @note The Length field in Marker Segment is in big-endian.
 * 
 * @param offset The pointer to the start of the Marker Segment
 * 
 * @return The Length field of the Marker Segment
 */
uint16_t get_segment_length(uint8_t *offset) {
    uint16_t length = 0;
    memcpy(&length, offset + 2, 2);
    return __builtin_bswap16(length);
}


int main() {
    FILE *img = fopen("./images/2023-0290.jpeg", "rb"); //TODO: if using debugger, change path to ../images/IMG_XXXX.jpeg
    uint8_t buffer[10240] = {0};
    fread(buffer, 10240, 1, img);

    /* Point to the first byte of the image */
    uint8_t *offset = buffer;

    /* Skip SOI (FF D8) */
    offset += 2;

    /**
     * Parse Marker Segment, since the number of Marker Segment is unknown, a list of 16 Marker Segment pointers will
     * be created.
     */
    Marker_Segment *segments[16] = {0}; /* list of Marker Segment pointers */
    uint8_t id[2] = {0};                /* the identifier of the current Marker Segment */
    uint16_t seg_len = 0;               /* the length of the current Marker Segment */
    uint8_t seg_idx = 0;                /* the index of the Marker Segment pointer in the list */

    do {
        /* Obtain the length of the Marker Segment */
        seg_len = get_segment_length(offset);

        /* Construct Marker Segment and skip it */
        segments[seg_idx] = calloc(1, sizeof(Marker_Segment));
        offset = construct_marker_segment(segments[seg_idx++], offset, seg_len);

        /**
         * Obtain the identifier of the Marker Segment. Since there is at least 1 Marker Segment, we only need to
         * check the existance of Marker Segment after constructing the first one.
         */
        memcpy(id, offset, 2);

    } while ( (id[1] & 0xF0) == 0xE0 );
}
