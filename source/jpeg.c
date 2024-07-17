#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

#include "jpeg.h"
#include "jfif.h"
#include "exif.h"


void jpeg_construct(struct JPEG *jpeg, uint8_t *ptr) {
    uint16_t marker = 0;

    /* Skip SOI Marker Segment, now pointing at APP Marker Segment */
    ptr += 2;

    while (1) {
        /* Parse Marker field */
        marker = __builtin_bswap16(*(uint16_t *)ptr);

        /* Construct the corresponding APP Marker Segment */
        switch (marker) {
            case 0xFFE0: {
                printf("APP0\n");
                jpeg->JFIF_Seg = calloc(1, sizeof(struct JFIF_Segment));
                jfif_construct(jpeg->JFIF_Seg, &ptr);
                break;
            }

            case 0xFFE1: {
                printf("APP1\n");
                jpeg->EXIF_Seg = calloc(1, sizeof(struct EXIF_Segment));
                exif_construct(jpeg->EXIF_Seg, &ptr);
                break;
            }

            default: {
                printf("Not an APP Marker Segment\n");
                return;
            }
        }
    }
}

void jpeg_parse(struct JPEG *jpeg) {
    jfif_parse(jpeg->JFIF_Seg);
    exif_parse(jpeg->EXIF_Seg);
}

void jpeg_free(struct JPEG *jpeg) {
    jfif_free(jpeg->JFIF_Seg);
    free(jpeg->JFIF_Seg);
    exif_free(jpeg->EXIF_Seg);
    free(jpeg->EXIF_Seg);
}
