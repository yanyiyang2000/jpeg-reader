#include <stdint.h> // uintXX_t
#include <stdlib.h> // size_t

#ifndef EXIF_H
#define EXIF_H

/**
 * [TIFF Rev 6.0, p13]
 * 
 * Byte order used with the file (Exif Marker Segment)
 */
#define BYTE_ORDER_LITTLE_ENDIAN 0x4949
#define BYTE_ORDER_BIG_ENDIAN    0x4D4D

/**
 * [TIFF Rev. 6.0, pp.15-16]
 * 
 * Field types
 */
#define BYTE       1    /* 8-bit unsigned integer */
#define ASCII      2    /* 8-bit byte that contains a 7-bit ASCII code; the last byte must be NUL */
#define SHORT      3    /* 16-bit (2-byte) unsigned integer */
#define LONG       4    /* 32-bit (4-byte) unsigned integer */
#define RATIONAL   5    /* Two LONGs: the first represents the numerator of a fraction, the second the denominator */
#define SBYTE      6    /* 8-bit signed (twos-complement) integer */
#define UNDEFINED  7    /* 8-bit byte that may contain anything, depending on the definition of the field. */
#define SSHORT     8    /* 16-bit (2-byte) signed (twos-complement) integer */
#define SLONG      9    /* 32-bit (4-byte) signed (twos-complement) integer */
#define SRATIONAL 10    /* Two SLONG’s: the first represents the numerator of a fraction, the second the denominator */
#define FLOAT     11    /* Single precision (4-byte) IEEE format */
#define DOUBLE    12    /* Double precision (8-byte) IEEE format */

/**
 * [TIFF Rev. 6.0, pp.14-15]
 * 
 * A 12-byte Directory Entry (DE) has the following format:
 * 
 *  - Bytes 0-1:  The Tag that identifies the field.
 * 
 *  - Bytes 2-3:  The field Type.
 * 
 *  - Bytes 4-7:  The number of values, Count of the indicated Type.
 * 
 *  - Bytes 8-11: The Value Offset, the file offset (in bytes) of the Value for the field. The Value is expected to 
 *                begin on a word boundary; the corresponding Value Offset will thus be an even number. This file offset 
 *                may point anywhere in the file, even after the image data.
 * 
 * @note The Count of an ASCII field entry includes the NULL byte. If padding is necessary, the Count does not include 
 *       the pad byte.
 */
struct __attribute__((packed)) Directory_Entry {
    uint16_t Tag;               /* The field tag */
    uint16_t Type;              /* The field type */
    uint32_t Value_Count;       /* The number of field values */
    union {
        uint32_t Value_Offset;  /* The offset of the field values */
        void     *Values;        /* The pointer to the field values */
    };
};

/**
 * [TIFF Rev. 6.0, p.14]
 * 
 * An Image File Directory (IFD) consists of a 2-byte count of the number of Directory Entries, followed by a sequence
 * of 12-byte Directory Entries, followed by a 4-byte offset of the next IFD (or 0 if none).
 * 
 * @note The last IFD is followed by 4 NULL bytes.
 * 
 */
struct Image_File_Directory {
    uint16_t                     DE_Count;      /* The number of Directory Entries */
    struct Directory_Entry       *DE;           /* The pointer to the first DE, for ease of programming */
    uint32_t                     IFD_Offset;    /* The offset (in bytes) of the next IFD from the Image File Header */
    struct Image_File_Directory  *Next_IFD;     /* The pointer to the next IFD, for ease of programming, not defined by TIFF */
};

/**
 * [TIFF Rev. 6.0, p.13]
 * 
 * An 8-byte Image File Header (IFH) has the following format:
 * 
 *  - Bytes 0-1: The byte order used within the file. (0x4949 for little-endian and 0x4D4D for big-endian)
 * 
 *  - Bytes 2-3: An arbitrary but carefully chosen number (42).
 * 
 *  - Bytes 4-7: The offset (in bytes) of the first IFD. The directory may be at any location in the file after the 
 *               header but must begin on a word boundary. In particular, an Image File Directory may follow the image 
 *               data it describes.
 */
struct __attribute__((packed)) Image_File_Header {
    uint16_t Byte_Order;    /* The byte order used within the file */
    uint16_t Magic_Number;  /* An arbitrary but carefully chosen number (42) */
    uint32_t IFD_Offset;    /* The offset of the first IFD in bytes */
};

/**
 * [Exif v3.0, p.31]
 * 
 * Exif Marker Segment
 */
struct __attribute__((packed)) Exif_Segment {
    uint8_t                     Identifier[6];  /* The identifier of the Segment. (i.e., Double NULL-terminated ASCII string "Exif\0\0") */
    struct Image_File_Header    IFH;            /* The Image File Header */
    struct Image_File_Directory *IFDs;          /* The pointer to the first IFD */
    struct Image_File_Directory *Exif_IFD;      /* The pointer to the Exif IFD */
    struct Image_File_Directory *GPS_IFD;       /* The pointer to the GPS IFD */
};

/**
 * This function performs byte swap on the fields of Directory Entry (DE).
 * 
 * @note Byte swap for Value Offset field will be performed by directory_entry_parse_value.
 * 
 * @param de The pointer to the DE
 */
void directory_entry_byte_swap(struct Directory_Entry *de);

/**
 * This function prints information of Directory Entry (DE).
 * 
 * @param de The pointer to the DE
 */
void directory_entry_print_info(struct Directory_Entry *de);

/**
 * This functions evaluates the Value Offset field of the DE and finds the actual Value if the Value Offset is stored.
 * 
 * @note If the Value is shorter than 4 bytes, it is left-justified within the 4-byte Value Offset. [TIFF Rev 6.0, p.15]
 * 
 * @param seg The pointer to the Exif Segment
 * @param de  The pointer to the DE
 * @param ifh The pointer to the Image File Header (IFD) of APP1 Marker Segment
 */
void directory_entry_parse_value(struct Exif_Segment *seg, struct Directory_Entry *de, uint8_t *ifh);

/**
 * This function prints information of Image File Directory (IFD).
 * 
 * @param ifd The pointer to the IFD
 */
void image_file_directory_print_info(struct Image_File_Directory *ifd);

/**
 * This function performs byte swap on the fields of Image File Header (IFH).
 * 
 * @param ifh The pointer to the IFH
 */
void image_file_header_byte_swap(struct Image_File_Header *ifh);

/**
 * This function constructs Directory Entries (DEs) in an Image File Directory (IFD) by parsing the given byte array.
 * 
 * There are two Exif-specific DEs:
 * 
 * - DE with Tag 0x8769 contains offset to the Exif IFD from the first byte of the Image File Header (IFH).
 * 
 * - DE with Tag 0x8825 contains offset to the GPS IFD from the first byte of the IFH.
 * 
 * @param seg The pointer to the Exif Segment
 * @param ptr The pointer to the pointer to the byte array to be parsed
 * @param ifh The pointer to the IFH
 * 
 * @note The parameter `ptr` will be advanced by the length of the DEs.
 */
void exif_construct_de(struct Exif_Segment *seg, struct Image_File_Directory *ifd, uint8_t **ptr, uint8_t *ifh);

/**
 * This function constructs an Image Field Directory (IFD) by parsing the given byte array.
 * 
 * @param seg The pointer to the Exif Segment
 * @param ifd The pointer to the IFD
 * @param ptr The pointer to the pointer to the byte array to be parsed
 * @param ifh The pointer to the Image File Header (IFH)
 * 
 * @note The parameter `ptr` will be advanced by the length of the IFD.
 */
void exif_construct_ifd(struct Exif_Segment *seg, struct Image_File_Directory *ifd, uint8_t **ptr, uint8_t *ifh);

/**
 * This function constructs an Exif Segment by parsing the given byte array.
 * 
 * @param seg     The pointer to the Exif Segment
 * @param ptr     The pointer to the pointer to the byte array to be parsed
 * @param seg_len The length of the Exif Segment
 * 
 * @note The parameter `ptr` will be advanced by the length of the Exif Segment.
 */
void exif_construct_segment(struct Exif_Segment *seg, uint8_t **ptr, uint16_t seg_len);

/**
 * This function frees the memory dynamically allocated to the given Exif Segment.
 * 
 * @param seg The pointer to the Exif Segment
 */
void exif_free_segment(struct Exif_Segment *seg);

#endif /* EXIF_H */
