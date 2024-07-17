#ifndef JPEG_H
#define JPEG_H

#include <stdint.h>

#include "jfif.h"
#include "exif.h"

struct JPEG {
    struct EXIF_Segment *EXIF_Seg;
    struct JFIF_Segment *JFIF_Seg;
};

/**
 * Construct a JPEG struct by parsing the given byte array.
 * 
 * @param seg The pointer to the JPEG struct
 * @param ptr The pointer to the byte array
 */
void jpeg_construct(struct JPEG *jpeg, uint8_t *ptr);

/**
 * Parse the given JPEG struct.
 * 
 * @param seg The pointer to the JPEG struct
 */
void jpeg_parse(struct JPEG *jpeg);

/**
 * Free the memory dynamically allocated to the given JPEG struct.
 * 
 * @param jpeg The pointer to the JPEG struct
 */
void jpeg_free(struct JPEG *jpeg);

#endif /* JPEG_H */
