#include <stdbool.h>  // true, false
#include <stdint.h>   // uintXX_t
#include <stdio.h>    // printf
#include <string.h>   // memcpy
#include <strings.h>  // bzero
#include <inttypes.h> // PRIX

#include "exif.h"

/* Flag indicating mismatch between the image and machine endianess. */
bool need_byte_swap = false;

void directory_entry_byte_swap(struct Directory_Entry *de) {
    de->Tag         = __builtin_bswap16(de->Tag);
    de->Type        = __builtin_bswap16(de->Type);
    de->Value_Count = __builtin_bswap32(de->Value_Count);
}

void directory_entry_print_info(struct Directory_Entry *de) {
    uint16_t tag         = de->Tag;
    uint16_t type        = de->Type;
    uint32_t value_count = de->Value_Count;

    if (type == BYTE) {
        uint8_t *ptr = de->uValue1;
        printf("0x%-.4X | %-6d | %-11d | %"PRIu8"\n", tag, type, value_count, *ptr);
        for (int i = 1; i < value_count; i++) {
            printf("%32"PRIu8"\n", *(ptr + i));
        }

    } else if (type == SBYTE) {
        int8_t *ptr = de->sValue1;
        printf("0x%-.4X | %-6d | %-11d | %"PRId8"\n", tag, type, value_count, *ptr);
        for (int i = 1; i < value_count; i++) {
            printf("%32"PRId8"\n", *(ptr + i));
        }

    } else if (type == ASCII) {
        printf("0x%-.4X | %-6d | %-11d | %s\n", tag, type, value_count, de->uValue1);

    } else if (type == SHORT) {
        uint16_t *ptr = de->uValue2;
        printf("0x%-.4X | %-6d | %-11d | %"PRIu16"\n", tag, type, value_count, *ptr);
        for (int i = 1; i < value_count; i++) {
            /* Print 32 spaces before printing the value */
            printf("%*c%"PRIu16"\n", 32, ' ', *(ptr + i));
        }

    } else if (type == SSHORT) {
        int16_t *ptr = de->sValue2;
        printf("0x%-.4X | %-6d | %-11d | %"PRId16"\n", tag, type, value_count, *ptr);
        for (int i = 1; i < value_count; i++) {
            printf("%32"PRId16"\n", *(ptr + i));
        }

    } else if (type == LONG) {
        uint32_t *ptr = de->uValue4;
        printf("0x%-.4X | %-6d | %-11d | %"PRIu32"\n", tag, type, value_count, *ptr);
        for (int i = 1; i < value_count; i++) {
            printf("%32"PRIu32"\n", *(ptr + i));
        }

    } else if (type == SLONG) {
        int32_t *ptr = de->sValue4;
        printf("0x%-.4X | %-6d | %-11d | %"PRId32"\n", tag, type, value_count, *ptr);
        for (int i = 1; i < value_count; i++) {
            printf("%32"PRId32"\n", *(ptr + i));
        }

    } else if (type == RATIONAL) {
        uint32_t *ptr = de->uValue4;
        printf("0x%-.4X | %-6d | %-11d | %"PRIu32"/%"PRIu32"\n", tag, type, value_count, *ptr, *(ptr + 1));
        for (int i = 1; i < value_count; i++) {
            /* Print 32 spaces before printing the value */
            printf("%*c%"PRIu32"/%"PRIu32"\n", 32, ' ', *(ptr + 2*i), *(ptr + 2*i + 1));
        }
    } else if (type == SRATIONAL) {
        int32_t *ptr = de->sValue4;
        printf("0x%-.4X | %-6d | %-11d | %"PRId32"/%"PRId32"\n", tag, type, value_count, *ptr, *(ptr + 1));
        for (int i = 1; i < value_count; i++) {
            printf("%32"PRId32"/%"PRId32"\n", *(ptr + 2*i), *(ptr + 2*i + 1));
        }
    } else if (type == FLOAT) {
        float *ptr = de->fValue4;
        printf("0x%-.4X | %-6d | %-11d | %f\n", tag, type, value_count, *ptr);
        for (int i = 1; i < value_count; i++) {
            printf("%32f\n", *(ptr + i));
        }
    } else if (type == DOUBLE) {
        double *ptr = de->fValue8;
        printf("0x%-.4X | %-6d | %-11d | %f\n", tag, type, value_count, *ptr);
        for (int i = 1; i < value_count; i++) {
            printf("%32f\n", *(ptr + i));
        }
    }
}

void directory_entry_parse_value(struct Directory_Entry *de, uint8_t *ifh) {
    uint16_t tag         = de->Tag;
    uint16_t type        = de->Type;
    uint32_t value_count = de->Value_Count;
    uint16_t offset      = 0;
    uint32_t value       = 0;

    /**
     * Obtain the Value or Offset of the Value by evaluating the Value Offset field.
     * 
     * If sizeof(Type) * Value_Count <= 4, replace the Value_Offset field with the data type (e.g., uValue1) indicated 
     * by the Type field, otherwise allocate memory for data type indicated by the Type field and copy the Value using
     * Value_Offset.
     */
    if ( (type == BYTE) | (type == ASCII) | (type == UNDEFINED) ) {
        if (value_count <= 4) {     // Value presents
            value = de->Value_Offset;
            de->Value_Offset = 0;   // clear the Value_Offset field for replacement
            de->uValue1 = calloc(value_count, 1);
            memcpy(de->uValue1, &value, value_count);

        } else {                    // Offset presents
            if (need_byte_swap) {
                offset = __builtin_bswap32(de->Value_Offset);
            } else {
                offset = de->Value_Offset;
            }
            de->Value_Offset = 0;   // clear the Value_Offset field
            de->uValue1 = calloc(value_count, 1);
            memcpy(de->uValue1, ifh + offset, value_count);
        }

    } else if (type == SBYTE) {
        if (value_count <= 4) {     // Value presents
            value = de->Value_Offset;
            de->Value_Offset = 0;   // clear the Value_Offset field for replacement
            de->sValue1 = calloc(value_count, 1);
            memcpy(de->sValue1, &value, value_count);

        } else {                    // Offset presents
            if (need_byte_swap) {
                offset = __builtin_bswap32(de->Value_Offset);
            } else {
                offset = de->Value_Offset;
            }
            de->Value_Offset = 0;   // clear the Value_Offset field
            de->sValue1 = calloc(value_count, 1);
            memcpy(de->sValue1, ifh + offset, value_count);
        }

    } else if (type == SHORT) {
        if (value_count <= 2) {     // Value presents
            value = de->Value_Offset;
            de->Value_Offset = 0;   // clear the Value_Offset field
            de->uValue2 = calloc(value_count, 2);
            memcpy(de->uValue2, &value, 2*value_count);

        } else {                    // Offset presents
            offset = de->Value_Offset;
            de->Value_Offset = 0;   // clear the Value_Offset field
            de->uValue2 = calloc(value_count, 2);
            memcpy(de->uValue2, ifh + offset, 2*value_count);
        }

        if (need_byte_swap) {
            for (int i = 0; i < value_count; i++) {
                *(de->uValue2 + i) = __builtin_bswap16(*(de->uValue2 + i));
            }
        }

    } else if (type == SSHORT) {
        if (value_count <= 2) {     // Value presents
            value = de->Value_Offset;
            de->Value_Offset = 0;   // clear the Value_Offset field
            de->sValue2 = calloc(value_count, 2);
            memcpy(de->sValue2, &value, 2*value_count);

        } else {                    // Offset presents
            offset = de->Value_Offset;
            de->Value_Offset = 0;   // clear the Value_Offset field
            de->sValue2 = calloc(value_count, 2);
            memcpy(de->sValue2, ifh + offset, 2*value_count);
        }

        if (need_byte_swap) {
            for (int i = 0; i < value_count; i++) {
                *(de->sValue2 + i) = __builtin_bswap16(*(de->sValue2 + i));
            }
        }

    } else if (type == LONG) {
        if (value_count <= 1) {     // Value presents
            value = de->Value_Offset;
            de->Value_Offset = 0;   // clear the Value_Offset field
            de->uValue4 = calloc(value_count, 4);
            memcpy(de->uValue4, &value, 4*value_count);

        } else {                    // Offset presents
            offset = de->Value_Offset;
            de->Value_Offset = 0;   // clear the Value_Offset field
            de->uValue4 = calloc(value_count, 4);
            memcpy(de->uValue4, ifh + offset, 4*value_count);
        }

        if (need_byte_swap) {
            for (int i = 0; i < value_count; i++) {
                *(de->uValue4 + i) = __builtin_bswap32(*(de->uValue4 + i));
            }
        }

    } else if (type == SLONG) {
        if (value_count <= 1) {     // Value presents
            value = de->Value_Offset;
            de->Value_Offset = 0;   // clear the Value_Offset field
            de->sValue4 = calloc(value_count, 4);
            memcpy(de->sValue4, &value, 4*value_count);

        } else {                    // Offset presents
            offset = de->Value_Offset;
            de->Value_Offset = 0;   // clear the Value_Offset field
            de->sValue4 = calloc(value_count, 4);
            memcpy(de->sValue4, ifh + offset, 4*value_count);
        }

        if (need_byte_swap) {
            for (int i = 0; i < value_count; i++) {
                *(de->sValue4 + i) = __builtin_bswap32(*(de->sValue4 + i));
            }
        }

    } else if (type == FLOAT) {
        if (value_count <= 1) {     // Value presents
            value = de->Value_Offset;
            de->Value_Offset = 0;   // clear the Value_Offset field
            de->fValue4 = calloc(value_count, 4);
            memcpy(de->fValue4, &value, 4*value_count);

        } else {                    // Offset presents
            offset = de->Value_Offset;
            de->Value_Offset = 0;   // clear the Value_Offset field
            de->fValue4 = calloc(value_count, 4);
            memcpy(de->fValue4, ifh + offset, 4*value_count);
        }

        if (need_byte_swap) {
            for (int i = 0; i < value_count; i++) {
                *(de->fValue4 + i) = __builtin_bswap32(*(de->fValue4 + i));
            }
        }

    } else if (type == RATIONAL) {
        /* Since a RATIONAL takes at least 8 bytes, we can only retrieve using the Offset. */
        if (need_byte_swap) {
            offset = __builtin_bswap32(de->Value_Offset);
        } else {
            offset = de->Value_Offset;
        }
        de->Value_Offset = 0;                   // clear the Value_Offset field
        de->uValue4 = calloc(2*value_count, 4); // a RATIONAL consists of two LONGs
        memcpy(de->uValue4, ifh + offset, 8*value_count);

        if (need_byte_swap) {
            for (int i = 0; i < 2*value_count; i++) {
                *(de->uValue4 + i) = __builtin_bswap32(*(de->uValue4 + i));
            }
        }

    } else if (type == SRATIONAL) {
        /* Since SRATIONAL takes at least 8 bytes, we can only retrieve using the Offset. */
        if (need_byte_swap) {
            offset = __builtin_bswap32(de->Value_Offset);
        } else {
            offset = de->Value_Offset;
        }
        de->Value_Offset = 0;                   // clear the Value_Offset field
        de->sValue4 = calloc(2*value_count, 4); // a SRATIONAL consists of two SLONGs
        memcpy(de->sValue4, ifh + offset, 8*value_count);

        if (need_byte_swap) {
            for (int i = 0; i < 2*value_count; i++) {
                *(de->sValue4 + i) = __builtin_bswap32(*(de->sValue4 + i));
            }
        }

    } else if (type == DOUBLE) {
        /* Since DOUBLE takes at least 8 bytes, we can only retrieve using the Offset. */
        if (need_byte_swap) {
            offset = __builtin_bswap32(de->Value_Offset);
        } else {
            offset = de->Value_Offset;
        }
        de->Value_Offset = 0; // clear the Value_Offset field
        de->fValue8 = calloc(value_count, 8);
        memcpy(de->fValue8, ifh + offset, value_count);

        if (need_byte_swap) {
            for (int i = 0; i < value_count; i++) {
                *(de->fValue8 + i) = __builtin_bswap64(*(de->fValue8 + i));
            }
        }
    }

    if (tag == 0x8769) {
        /* Construct Exif IFD and print information */
        struct Image_File_Directory *exif_ifd = calloc(1, sizeof(struct Image_File_Directory));
        uint8_t *from = ifh + *(de->uValue4);
        exif_construct_ifd(exif_ifd, from, ifh);
        image_file_directory_print_info(exif_ifd);
    } else if (tag == 0x8825) {
        /* TODO: Construct GPS IFD and print information */
    }
}

void image_file_header_byte_swap(struct Image_File_Header *ifh) {
    ifh->Magic_Number = __builtin_bswap16(ifh->Magic_Number);
    ifh->IFD_Offset   = __builtin_bswap32(ifh->IFD_Offset);
}

void image_file_directory_print_info(struct Image_File_Directory *ifd) {
    printf("%-6s | %-6s | %-6s | %-6s\n", "Tag", "Type", "Value Count", "Value / Value Offset");
    for (int i = 0; i < ifd->DE_Count; i++) {
        directory_entry_print_info(ifd->DE + i);
    }
    printf("\n");
}

uint8_t* exif_construct_de(struct Directory_Entry *to, uint8_t *from, uint16_t de_count, uint8_t *ifh) {
    uint8_t *ptr = from;

    /* Obtain each DE */
    for (int i = 0; i < de_count; i++) {
        memcpy(to + i, ptr, 12);
        if (need_byte_swap) {
            directory_entry_byte_swap(to + i);
        }
        directory_entry_parse_value(to + i, ifh);
        ptr += 12;
    }

    return ptr;
}

uint8_t* exif_construct_ifd(struct Image_File_Directory *to, uint8_t *from, uint8_t *ifh) {
    uint8_t *ptr        = from; // pointer to the current byte
    uint16_t de_count   = 0;    // count of DE
    uint32_t ifd_offset = 0;    // offset of IFD

    struct Directory_Entry *de = NULL; // pointer to the current DE

    /* Obtain the DE Count field of the IFD */
    memcpy(&de_count, ptr, 2);
    if (need_byte_swap) {
        de_count = __builtin_bswap16(de_count);
    }
    to->DE_Count = de_count;

    /* Skip the DE Count field, now pointing at the first Directory Entry (DE) */
    ptr += 2; 

    /* Construct the DEs */
    de     = calloc(de_count, sizeof(struct Directory_Entry));
    ptr    = exif_construct_de(de, ptr, de_count, ifh);
    to->DE = de;

    /* Obtain the IFD Offset field of the IFD */
    memcpy(&ifd_offset, ptr, 4);
    if (need_byte_swap) {
        ifd_offset = __builtin_bswap32(ifd_offset);
    }
    to->IFD_Offset = ifd_offset;

    /**
     * Set the offset to point to the next IFD if exists, otherwise it means that there is no more IFD and hence the 
     * Marker Segment ends.
     */
    if (ifd_offset > 0) {
        ptr = ifh + ifd_offset; // skip this IFD, now pointing at the next IFD
    } // if this is the last IFD, offset value would not be useful so nothing is done here

    return ptr;
}

uint8_t* exif_construct_segment(struct Exif_Segment *to, uint8_t *from, uint16_t seg_len) {
    uint8_t *ptr = from; // pointer to the current byte
    uint8_t *ifh = NULL; // pointer to the first byte of the IFH

    struct Image_File_Directory *ifd      = NULL; // pointer to the first IFD
    struct Image_File_Directory *curr_ifd = NULL; // pointer to the current IFD
    struct Image_File_Directory *next_ifd = NULL; // pointer to the next IFD

    /* Obtain the Identifier field of the Exif Segment */
    memcpy(to->Identifier, ptr, 6);

    /* Skip the Identifier field, now pointing at the Byte Order field */
    ptr += 6;

    /* Obtain the Image File Header */
    ifh = ptr;
    memcpy(&(to->IFH), ptr, 8);
    if (to->IFH.Byte_Order == BYTE_ORDER_BIG_ENDIAN) {
        need_byte_swap = true;
        image_file_header_byte_swap(&(to->IFH));
    }

    /* Skip the IFH and IFD offset if exists, now pointing at the 1st IFD */
    ptr += to->IFH.IFD_Offset;

    /* Construct the 1st IFD and print the information */
    ifd = calloc(1, sizeof(struct Image_File_Directory));
    ptr = exif_construct_ifd(ifd, ptr, ifh); // pointing at the next IFD if exists
    image_file_directory_print_info(ifd);
    
    /* Construct the rest IFDs if exist and print the information */
    curr_ifd = ifd; 
    while (curr_ifd->IFD_Offset != 0) { // The IFD Offset field of the last IFD is 0
        next_ifd = curr_ifd->Next_IFD;
        next_ifd = calloc(1, sizeof(struct Image_File_Directory));
        ptr = exif_construct_ifd(next_ifd, ptr, ifh);
        image_file_directory_print_info(next_ifd);
        curr_ifd = next_ifd;
    }

    /* Reset the offset to make it point to the next Marker Segment */
    ptr = from + seg_len;

    return ptr;
}
