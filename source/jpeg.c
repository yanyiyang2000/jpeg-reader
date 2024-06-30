#include <stdint.h> // uintXX_t
#include <string.h> // memcpy

#include "jpeg.h"
#include "jfif.h"
#include "exif.h"


uint8_t* construct_marker_segment(struct Marker_Segment *to, uint8_t *from, uint16_t seg_len) {
    uint8_t             *offset       = from;
    struct JFIF_Segment *jfif_segment = NULL;
    struct Exif_Segment *exif_segment = NULL;

    /* Obtain the Marker field */
    memcpy(to->Marker, offset, 2);
    offset += 2; // skip the Marker field, now pointing at the Length field */

    /* Assign value to the Length field */
    to->Length = seg_len;
    offset += 2; // skip the Length field, now pointing at the Identifier field */

    /* Construct a specific Segment based on the Marker field */
    switch (to->Marker[1]) {
        
        case 0xE0:
            jfif_segment = calloc(1, sizeof(struct JFIF_Segment));
            offset = jfif_construct_segment(jfif_segment, offset, seg_len-2);
            to->jfif_segment = jfif_segment;
            break;
        case 0xE1:
            exif_segment = calloc(1, sizeof(struct Exif_Segment));
            offset = exif_construct_segment(exif_segment, offset, seg_len-2);
            to->exif_segment = exif_segment;
            break;
    }

    return offset;
}
