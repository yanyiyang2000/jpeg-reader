#ifndef JFIF_H
#define JFIF_H

#include <stdint.h>

struct JFIF_Segment {
    uint8_t *Base;  // The pointer to the first byte of Length field of the JFIF Segment
};

/**
 * Construct a JFIF Segment struct by parsing the given byte array.
 * 
 * @param seg The pointer to the JFIF Segment struct
 * @param ptr The pointer to the pointer to the byte array
 * 
 * @note Parameter `ptr` will be advanced by the length of the JFIF Segment.
 */
void jfif_construct(struct JFIF_Segment *seg, uint8_t **ptr);

/**
 * Parse the given JFIF Segment struct.
 * 
 * @param seg The pointer to the JFIF Segment struct
 */
void jfif_parse(struct JFIF_Segment *seg);

/**
 * Free the memory dynamically allocated to the given JFIF Segment struct.
 * 
 * @param seg The pointer to the JFIF Segment struct
 */
void jfif_free(struct JFIF_Segment *seg);

#endif /* JFIF_H */
