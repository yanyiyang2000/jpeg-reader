#ifndef EXIF_H
#define EXIF_H

#include <stdint.h>


#define BYTE_ORDER_LITTLE_ENDIAN 0x4949
#define BYTE_ORDER_BIG_ENDIAN    0x4D4D

struct EXIF_Segment {
    uint8_t *IFH_Base;                      // The pointer to the first byte of IFH
    struct Image_File_Directory *Next_IFD;  // The pointer to the next IFD
    struct Image_File_Directory *EXIF_IFD;  // The pointer to the EXIF IFD
    struct Image_File_Directory *GPS_IFD;   // The pointer to the GPS IFD
};

struct Image_File_Directory {
    uint8_t  *IFD_Base;                     // The pointer to the first byte of DE Count field of the IFD
    uint16_t DE_Count;                      // The number of DEs
    struct Image_File_Directory *Next_IFD;  // The pointer to the next IFD
};

/**
 * Construct an EXIF Segment struct by parsing the given byte array.
 * 
 * @param seg The pointer to the EXIF Segment struct
 * @param ptr The pointer to the pointer to the byte array
 * 
 * @note Parameter `ptr` will be advanced by the length of the EXIF Segment.
 */
void exif_construct(struct EXIF_Segment *seg, uint8_t **ptr);

/**
 * Parse the given EXIF Segment struct.
 * 
 * @param seg The pointer to the EXIF Segment struct
 */
void exif_parse(struct EXIF_Segment *seg);

/**
 * Free the memory dynamically allocated to the given EXIF Segment struct.
 * 
 * @param seg The pointer to the EXIF Segment struct
 */
void exif_free(struct EXIF_Segment *seg);

#endif /* EXIF_H */
