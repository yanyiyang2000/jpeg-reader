#include <stdint.h> // uintXX_t
#include <stdlib.h> // size_t

#ifndef EXIF_H
#define EXIF_H

/**
 * [TIFF Rev 6.0, p13]
 * Byte order used with the file (Exif Marker Segment)
 */
#define BYTE_ORDER_LITTLE_ENDIAN 0x4949
#define BYTE_ORDER_BIG_ENDIAN    0x4D4D

/**
 * [TIFF Rev 6.0, p15-16]
 * Field types
 */
#define BYTE       1 /* 8-bit unsigned integer */
#define ASCII      2 /* 8-bit byte that contains a 7-bit ASCII code; the last byte must be NUL */
#define SHORT      3 /* 16-bit (2-byte) unsigned integer */
#define LONG       4 /* 32-bit (4-byte) unsigned integer */
#define RATIONAL   5 /* Two LONGs: the first represents the numerator of a fraction, the second the denominator */
#define SBYTE      6 /* 8-bit signed (twos-complement) integer */
#define UNDEFINED  7 /* 8-bit byte that may contain anything, depending on the definition of the field. */
#define SSHORT     8 /* 16-bit (2-byte) signed (twos-complement) integer */
#define SLONG      9 /* 32-bit (4-byte) signed (twos-complement) integer */
#define SRATIONAL 10 /* Two SLONG’s: the first represents the numerator of a fraction, the second the denominator */
#define FLOAT     11 /* Single precision (4-byte) IEEE format */
#define DOUBLE    12 /* Double precision (8-byte) IEEE format */

/**
 * [TIFF Rev 6.0, p14-15]
 * Each 12-byte Directory Entry (DE) has the following format:
 * - Bytes 0-1:  The Tag that identifies the field.
 * - Bytes 2-3:  The field Type.
 * - Bytes 4-7:  The number of values, Count of the indicated Type.
 * - Bytes 8-11: The Value Offset, the file offset (in bytes) of the Value for the field. The Value is expected to begin 
 *               on a word boundary; the corresponding Value Offset will thus be an even number. This file offset may 
 *               point anywhere in the file, even after the image data.
 * 
 * The value of the Count part of an ASCII field entry includes the NUL. If padding is necessary, the Count does not 
 * include the pad byte.
 */
typedef struct __attribute__((packed)) {
    uint16_t Tag;          /* Bytes 0-1 */
    uint16_t Type;         /* Bytes 2-3 */
    uint32_t Value_Count;  /* Bytes 4-7 */
    union {
        uint32_t Value_Offset; /* Bytes 8-11 */

        /* The following is for the ease of programming when evaluating the Value */
        uint8_t  *uValue1; /* stores a sequence of unsigned int Values each 1 byte,  i.e., Type BYTE, ASCII, SBYTE, UNDEFINED */
        uint16_t *uValue2; /* stores a sequence of unsigned int Values each 2 bytes, i.e., Type SHORT */
        uint32_t *uValue4; /* stores a sequence of unsigned int Values each 4 bytes, i.e., Type LONG, RATIONAL */
        int8_t   *sValue1; /* stores a sequence of signed int Values each 1 byte,  i.e., Type SBYTE */
        int16_t  *sValue2; /* stores a sequence of signed int Values each 2 bytes, i.e., Type SSHORT */
        int32_t  *sValue4; /* stores a sequence of signed int Values each 4 bytes, i.e., Type SLONG, SRATIONAL */
        float    *fValue4; /* stores a sequence of single precision floating Values each 4 bytes, i.e., Type FLOAT */
        double   *fValue8; /* stores a sequence of double precision floating Values each 8 bytes, i.e., Type DOUBLE */
    };
} Directory_Entry;

/**
 * [TIFF Rev 6.0, p14]
 * An Image File Directory (IFD) consists of a 2-byte count of the number of Directory Entries, followed by a sequence 
 * of 12-byte Directory Entries, followed by a 4-byte offset of the next IFD (or 0 if none).
 * 
 * @note 4 bytes of 0 are followed after the last IFD.
 */
struct Image_File_Directory {
    uint16_t                     DE_Count;   /* The number of directory entries */
    Directory_Entry              *DE;        /* Pointer to the first DE, for ease of programming */
    uint32_t                     IFD_Offset; /* The offset (in bytes) of the next IFD from the Image File Header */
    struct Image_File_Directory *Next_IFD;  /* Pointer to the next IFD, for ease of programming, not defined by TIFF */
};

/**
 * [TIFF Rev 6.0, p13]
 * An 8-byte Image File Header (IFH) contains the following information:
 * - Bytes 0-1: The byte order used within the file. Legal values are: 
 *                0x4949 (little-endian)
 *                0x4D4D (big-endian)
 * - Bytes 2-3: An arbitrary but carefully chosen number (42)
 * - Bytes 4-7: The offset (in bytes) of the first IFD. The directory may be at any location in the file after the header 
 *              but must begin on a word boundary. In particular, an Image File Directory may follow the image data it 
 *              describes.
 */
typedef struct __attribute__((packed)) {
    uint16_t Byte_Order;   /* Bytes 0-1 */
    uint16_t Magic_Number; /* Bytes 2-3 */
    uint32_t IFD_Offset;   /* Bytes 4-7 */
} Image_File_Header;

/**
 * [Exif v2.32, p19]
 * Exif Marker Segment
 */
typedef struct __attribute__((packed)) {
    uint8_t                     Identifier[6]; /* Double NULL terminated ASCII string "Exif\0\0" */
    Image_File_Header           IFH;           /* Image file header */
    struct Image_File_Directory *IFDs;         /* Pointer to the first IFDs */
} Exif_Segment;

void directory_entry_byte_swap(Directory_Entry *de);

void directory_entry_print_info(Directory_Entry *de);

void directory_entry_parse_value(Directory_Entry *de, uint8_t *ifh);

void image_file_header_byte_swap(Image_File_Header *header);

void image_file_directory_print_info(struct Image_File_Directory *ifd);

uint8_t* exif_construct_de(Directory_Entry *to, uint8_t *from, uint16_t de_count, uint8_t *ifh);

uint8_t* exif_construct_ifd(struct Image_File_Directory *to, uint8_t *from, uint8_t *ifh);

uint8_t* exif_construct_segment(Exif_Segment *to, uint8_t *from, uint16_t seg_len);

#endif /* EXIF_H */
