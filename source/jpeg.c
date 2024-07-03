#include <stdint.h> // uintXX_t
#include <string.h> // memcpy

#include "jpeg.h"
#include "jfif.h"
#include "exif.h"


uint8_t* construct_marker_segment(struct Marker_Segment *to, uint8_t *from, uint16_t seg_len) {
    uint8_t             *ptr          = from; // pointer to the current byte
    struct JFIF_Segment *jfif_segment = NULL; // pointer to the JFIF Segment
    struct Exif_Segment *exif_segment = NULL; // pointer to the Exif Segment

    /* Obtain the Marker field */
    memcpy(to->Marker, ptr, 2);

    /* Skip the Marker field, now pointing at the Length field */
    ptr += 2;

    /* Assign value to the Length field */
    to->Length = seg_len;

    /* Skip the Length field, now pointing at the Identifier field */
    ptr += 2;

    /* Construct a specific Segment based on the Marker field */
    switch (to->Marker[1]) {
        case 0xE0:
            jfif_segment = calloc(1, sizeof(struct JFIF_Segment));
            ptr = jfif_construct_segment(jfif_segment, ptr, seg_len-2);
            to->jfif_segment = jfif_segment;
            break;

        case 0xE1:
            exif_segment = calloc(1, sizeof(struct Exif_Segment));
            ptr = exif_construct_segment(exif_segment, ptr, seg_len-2);
            to->exif_segment = exif_segment;
            break;
    }

    return ptr;
}
