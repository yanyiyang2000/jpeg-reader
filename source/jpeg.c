#include <stdint.h> // uintXX_t
#include <string.h> // memcpy

#include "jpeg.h"
#include "jfif.h"
#include "exif.h"


void construct_marker_segment(struct Marker_Segment *seg, uint8_t **ptr, uint16_t seg_len) {
    struct JFIF_Segment *jfif_segment = NULL; // pointer to the JFIF Segment
    struct Exif_Segment *exif_segment = NULL; // pointer to the Exif Segment

    /* Obtain the Marker field */
    memcpy(seg->Marker, *ptr, 2);

    /* Skip the Marker field, now pointing at the Length field */
    *ptr += 2;

    /* Assign value to the Length field */
    seg->Length = seg_len;

    /* Skip the Length field, now pointing at the Identifier field */
    *ptr += 2;

    /* Construct a specific Segment based on the Marker field and skip it */
    switch (seg->Marker[1]) {
        case 0xE0:
            jfif_segment = calloc(1, sizeof(struct JFIF_Segment));
            jfif_construct_segment(jfif_segment, ptr, seg_len - 2);
            seg->jfif_segment = jfif_segment;
            break;

        case 0xE1:
            exif_segment = calloc(1, sizeof(struct Exif_Segment));
            exif_construct_segment(exif_segment, ptr, seg_len - 2);
            seg->exif_segment = exif_segment;
            break;
    }
}

void free_marker_segment(struct Marker_Segment *seg) {
    switch (seg->Marker[1]) {
        case 0xE0:
            jfif_free_segment(seg->jfif_segment);
            break;

        case 0xE1:
            exif_free_segment(seg->exif_segment); // TODO: seg fault
            break;
    }

    free(seg);
}
