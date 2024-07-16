#include <stdio.h>   // fopen, fclose, fread
#include <stdint.h>  // uint8_t
#include <stdlib.h>  // calloc, free
#include <string.h>  // memcpy

#include "jpeg.h"


int main(int argc, char *argv[]) {
    FILE                  *img      = NULL; // file descriptor of the image
    uint8_t               *buf      = NULL; // pointer to the byte array containing image data
    uint8_t               *ptr      = NULL; // pointer to the current byte
    struct Marker_Segment *segs[16] = {0};  // list of Marker Segment pointers, 16 for now
    uint8_t               id[2]     = {0};  // identifier of the current Marker Segment
    uint8_t               seg_idx   = 0;    // index of the Marker Segment pointer in the list

    /* Allocate dynamic memory */
    buf = calloc(1024000, 1);
    for (uint8_t i = 0; i < 16; i++) {
        segs[i] = calloc(1, sizeof(struct Marker_Segment));
    }

    /* Assert argument */
    if (argc != 2) {
        printf("Usage: test <FILE_NAME>");
        return 1;
    }

    /* Read image data */
    img = fopen(argv[1], "rb");
    fread(buf, 1024000, 1, img);
    
    /* Set the pointer to point to the first byte of the image data */
    ptr = buf;

    /* Skip the SOI Marker (FF D8) */
    ptr += 2;

    /* Parse the Marker Segments */
    do {
        /* Construct Marker Segment and skip it */
        construct_marker_segment(segs[seg_idx++], &ptr);

        /**
         * Obtain the identifier of the next Marker Segment. Since there is at least 1 Marker Segment, we only need to 
         * check the existance of Marker Segment after constructing the first one.
         */
        memcpy(id, ptr, 2);

    } while ( (id[1] & 0xF0) == 0xE0 );

    fclose(img);

    /* Free all dynamically allocated memory */
    free(buf);
    for (uint8_t i = 0; i < 16; i++) {
        if (segs[i] != NULL) {
            free_marker_segment(segs[i]);
        }
    }
}
