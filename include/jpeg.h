/**
 * @file   jpeg.h
 * 
 * @author Yiyang Yan
 * 
 * @date   2024/07/20
 * 
 * @brief  User API to extract metadata from JPEG files.
 */

#ifndef JPEG_H
#define JPEG_H

#include <stdint.h>

#include "jfif.h"
#include "exif.h"

/**
 * @brief JPEG file representation
 */
struct JPEG {
    struct EXIF_Segment *EXIF_Seg;  // The pointer to the EXIF Segment
    struct JFIF_Segment *JFIF_Seg;  // The pointer to the JFIF Segment
};

/**
 * @brief Construct a JPEG struct by parsing the given byte array.
 * 
 * @param seg The pointer to the JPEG struct
 * @param ptr The pointer to the byte array
 */
void jpeg_construct(struct JPEG *jpeg, uint8_t *ptr);

/**
 * @brief Parse the given JPEG struct.
 * 
 * @param seg The pointer to the JPEG struct
 */
void jpeg_parse(struct JPEG *jpeg);

/**
 * @brief Free the memory dynamically allocated to the given JPEG struct.
 * 
 * @param jpeg The pointer to the JPEG struct
 */
void jpeg_free(struct JPEG *jpeg);

#endif /* JPEG_H */
