/**
 * @file   jpeg.h
 * 
 * @author Yiyang Yan
 * 
 * @date   2024/07/20
 * 
 * @brief  Functions to parse JFIF Segment.
 */

#ifndef JFIF_H
#define JFIF_H

#include <stdint.h>

/**
 * @brief JFIF Segment representation
 */
struct JFIF_Segment {
    uint8_t *Base;  // The pointer to the first byte of VERSION MAJOR of the JFIF Segment
};

/**
 * @brief Construct a JFIF Segment struct by parsing the given byte array.
 * 
 * @param seg The pointer to the JFIF Segment struct
 * @param ptr The pointer to the pointer to the byte array
 * 
 * @note Parameter `ptr` will be advanced by the length of the JFIF Segment.
 */
void jfif_construct(struct JFIF_Segment *seg, uint8_t **ptr);

/**
 * @brief Parse the given JFIF Segment struct.
 * 
 * @param seg The pointer to the JFIF Segment struct
 */
void jfif_parse(struct JFIF_Segment *seg);

/**
 * @brief Free the memory dynamically allocated to the given JFIF Segment struct.
 * 
 * @param seg The pointer to the JFIF Segment struct
 */
void jfif_free(struct JFIF_Segment *seg);

#endif /* JFIF_H */
