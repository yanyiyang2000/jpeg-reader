#include <stdbool.h>  // true, false
#include <stdint.h>   // uintXX_t, intXX_t
#include <stdio.h>    // printf
#include <string.h>   // memcpy
#include <strings.h>  // bzero
#include <inttypes.h> // PRIX

#include "exif.h"
#include "tag.h"


/* Flag indicating mismatch between the image and machine endianess. */
bool need_byte_swap = false;

void directory_entry_byte_swap(struct Directory_Entry *de) {
    de->Tag         = __builtin_bswap16(de->Tag);
    de->Type        = __builtin_bswap16(de->Type);
    de->Value_Count = __builtin_bswap32(de->Value_Count);
}

void directory_entry_print_info(struct Directory_Entry *de) {
    uint16_t tag         = de->Tag;
    uint32_t type        = de->Type;
    uint32_t value_count = de->Value_Count;

    char *tag_name  = NULL; // field tag name

    /* Obtain the field tag name */
    for (uint8_t i = 0; i < (sizeof(tags)/sizeof(struct Tag)); i++) {
        if (tags[i].Number == tag) {
            tag_name = tags[i].Name;
            break;
        }
    }

    /* In case of unknown field tag number, print the tag number in hex */
    if (tag_name == NULL) {
        printf("│ 0x%-25"PRIX16" │ %-9s │ %-5s │ %-49s │\n", tag, "", "", "");
    }

    /* Print the field tag name, field type, field value count and field values */
    if (type == BYTE) {

        uint8_t *ptr = de->Value;
        printf("│ %-27s │ %-9s │ %-5"PRIu32" │ %-49"PRIu8" │\n", tag_name, "BYTE", value_count, *ptr);
        for (uint8_t i = 1; i < value_count; i++) {
            printf("│ %-27s | %-9s │ %-5s │ %-49"PRIu8" │\n", "", "", "", *(ptr + i));
        }

    } else if (type == SBYTE) {

        int8_t *ptr = de->Value;
        printf("│ %-27s │ %-9s │ %-5"PRIu32" │ %-49"PRId8" │\n", tag_name, "SBYTE", value_count, *ptr);
        for (uint8_t i = 1; i < value_count; i++) {
            printf("│ %-27s | %-9s │ %-5s │ %-49"PRId8" │\n", "", "", "", *(ptr + i));
        }

    } else if (type == ASCII) {

        printf("│ %-27s │ %-9s │ %-5"PRIu32" │ %-49s │\n", tag_name, "ASCII", value_count, (char *)(de->Value));

    } else if (type == SHORT) {

        uint16_t *ptr = de->Value;
        printf("│ %-27s │ %-9s │ %-5"PRIu32" │ %-49"PRIu16" │\n", tag_name, "SHORT", value_count, *ptr);
        for (uint8_t i = 1; i < value_count; i++) {
            printf("│ %-27s | %-9s │ %-5s │ %-49"PRIu16" │\n", "", "", "", *(ptr + i));
        }

    } else if (type == SSHORT) {

        int16_t *ptr = de->Value;
        printf("│ %-27s │ %-9s │ %-5"PRIu32" │ %-49"PRId16" │\n", tag_name, "SSHORT", value_count, *ptr);
        for (uint8_t i = 1; i < value_count; i++) {
            printf("│ %-27s | %-9s │ %-5s │ %-49"PRId16" │\n", "", "", "", *(ptr + i));
        }

    } else if (type == LONG) {

        uint32_t *ptr = de->Value;
        printf("│ %-27s │ %-9s │ %-5"PRIu32" │ %-49"PRIu32" │\n", tag_name, "LONG", value_count, *ptr);
        for (uint8_t i = 1; i < value_count; i++) {
            printf("│ %-27s | %-9s │ %-5s │ %-49"PRIu32" │\n", "", "", "", *(ptr + i));
        }

    } else if (type == SLONG) {

        int32_t *ptr = de->Value;
        printf("│ %-27s │ %-9s │ %-5"PRIu32" │ %-49"PRId32" │\n", tag_name, "SLONG", value_count, *ptr);
        for (uint8_t i = 1; i < value_count; i++) {
            printf("│ %-27s | %-9s │ %-5s │ %-49"PRId32" │\n", "", "", "", *(ptr + i));
        }

    } else if (type == RATIONAL) {

        uint32_t *ptr = de->Value;
        /* Uncomment to print in fraction */
        // printf("│ %-27s │ %-9s │ %-5"PRIu32" │ %24"PRIu32"/%-24"PRIu32" │\n", tag_name, "RATIONAL", value_count, *ptr, *(ptr + 1));
        printf("│ %-27s │ %-9s │ %-5"PRIu32" │ %-49.2f │\n", tag_name, "RATIONAL", value_count, (double)(*ptr)/(*(ptr + 1)));
        for (uint8_t i = 1; i < value_count; i++) {
            // printf("│ %-27s | %-9s │ %-5s │ %24"PRIu32"/%-24"PRIu32" │\n", "", "", "", *(ptr + i), *(ptr + 2*i + 1));
            printf("│ %-27s | %-9s │ %-5s │ %-49.2f │\n", "", "", "", (double)(*(ptr + i))/(*(ptr + 2*i + 1)));
        }

    } else if (type == SRATIONAL) {

        int32_t *ptr = de->Value;
        /* Uncomment to print in fraction */
        // printf("│ %-27s │ %-9s │ %-5"PRIu32" │ %24"PRId32"/%-24"PRId32" │\n", tag_name, "SRATIONAL", value_count, *ptr, *(ptr + 1));
        printf("│ %-27s │ %-9s │ %-5"PRIu32" │ %-49.2f │\n", tag_name, "SRATIONAL", value_count, (double)(*ptr)/(*(ptr + 1)));
        for (uint8_t i = 1; i < value_count; i++) {
            // printf("│ %-27s | %-9s │ %-5s │ %24"PRId32"/%-24"PRId32" │\n", "", "", "", *(ptr + i), *(ptr + 2*i + 1));
            printf("│ %-27s | %-9s │ %-5s │ %-49.2f │\n", "", "", "", (double)(*(ptr + i))/(*(ptr + 2*i + 1)));
        }

    } else if (type == FLOAT) {

        float *ptr = de->Value;
        printf("│ %-27s │ %-9s │ %-5"PRIu32" │ %-49f │\n", tag_name, "FLOAT", value_count, *ptr);
        for (uint8_t i = 1; i < value_count; i++) {
            printf("│ %-27s | %-9s │ %-5s │ %-49f │\n", "", "", "", *(ptr + i));
        }
        
    } else if (type == DOUBLE) {

        double *ptr = de->Value;
        printf("│ %-27s │ %-9s │ %-5"PRIu32" │ %-49f │\n", tag_name, "DOUBLE", value_count, *ptr);
        for (uint8_t i = 1; i < value_count; i++) {
            printf("│ %-27s | %-9s │ %-5s │ %-49f │\n", "", "", "", *(ptr + i));
        }

    } else if (type == UNDEFINED) {
        printf("│ %-27s │ %-9s │ %-5"PRIu32" │ %-49s │\n", tag_name, "UNDEFINED", value_count, "");
    }
}

void directory_entry_parse_value(struct Directory_Entry *de, uint8_t *ifh) {
    uint16_t tag         = de->Tag;
    uint16_t type        = de->Type;
    uint32_t value_count = de->Value_Count;

    uint32_t offset      = 0; // value offset
    uint32_t value       = 0; // actual value

    /**
     * Obtain the Value or Offset of the Value by evaluating the Value Offset field.
     * 
     * If sizeof(Type) * Value_Count <= 4, replace the Value_Offset field with the actual data, 
     * otherwise allocate memory for data type indicated by the Type field and copy the Value using Value_Offset.
     */
    if ( (type == BYTE) | (type == ASCII) | (type == UNDEFINED) ) {

        if (value_count <= 4) {     // Value presents
            value = de->Value_Offset;
            de->Value_Offset = 0;
            de->Value = calloc(value_count, 1);
            memcpy(de->Value, &value, value_count);
        } else {                    // Offset presents
            if (need_byte_swap) {
                offset = __builtin_bswap32(de->Value_Offset);
            } else {
                offset = de->Value_Offset;
            }
            de->Value_Offset = 0;
            de->Value = calloc(value_count, 1);
            memcpy(de->Value, ifh + offset, value_count);
        }

    } else if (type == SBYTE) {

        if (value_count <= 4) {     // Value presents
            value = de->Value_Offset;
            de->Value_Offset = 0;
            de->Value = calloc(value_count, 1);
            memcpy(de->Value, &value, value_count);
        } else {                    // Offset presents
            if (need_byte_swap) {
                offset = __builtin_bswap32(de->Value_Offset);
            } else {
                offset = de->Value_Offset;
            }
            de->Value_Offset = 0;
            de->Value = calloc(value_count, 1);
            memcpy(de->Value, ifh + offset, value_count);
        }

    } else if (type == SHORT) {

        if (value_count <= 2) {     // Value presents
            value = de->Value_Offset;
            de->Value_Offset = 0;
            de->Value = calloc(value_count, 2);
            memcpy(de->Value, &value, 2*value_count);
        } else {                    // Offset presents
            if (need_byte_swap) {
                offset = __builtin_bswap32(de->Value_Offset);
            } else {
                offset = de->Value_Offset;
            }
            de->Value_Offset = 0;
            de->Value = calloc(value_count, 2);
            memcpy(de->Value, ifh + offset, 2*value_count);
        }

        if (need_byte_swap) {
            for (int i = 0; i < value_count; i++) {
                *((uint16_t *)(de->Value + i)) = __builtin_bswap16( *((uint16_t *)(de->Value + i)) );
            }
        }

    } else if (type == SSHORT) {

        if (value_count <= 2) {     // Value presents
            value = de->Value_Offset;
            de->Value_Offset = 0;
            de->Value = calloc(value_count, 2);
            memcpy(de->Value, &value, 2*value_count);
        } else {                    // Offset presents
            if (need_byte_swap) {
                offset = __builtin_bswap32(de->Value_Offset);
            } else {
                offset = de->Value_Offset;
            }
            de->Value_Offset = 0;
            de->Value = calloc(value_count, 2);
            memcpy(de->Value, ifh + offset, 2*value_count);
        }

        if (need_byte_swap) {
            for (int i = 0; i < value_count; i++) {
                *((int16_t *)(de->Value + i)) = __builtin_bswap16( *((int16_t *)(de->Value + i)) );
            }
        }

    } else if (type == LONG) {

        if (value_count <= 1) {     // Value presents
            value = de->Value_Offset;
            de->Value_Offset = 0;
            de->Value = calloc(value_count, 4);
            memcpy(de->Value, &value, 4*value_count);
        } else {                    // Offset presents
            if (need_byte_swap) {
                offset = __builtin_bswap32(de->Value_Offset);
            } else {
                offset = de->Value_Offset;
            }
            de->Value_Offset = 0;
            de->Value = calloc(value_count, 4);
            memcpy(de->Value, ifh + offset, 4*value_count);
        }

        if (need_byte_swap) {
            for (int i = 0; i < value_count; i++) {
                *((uint32_t *)(de->Value + i)) = __builtin_bswap32( *((uint32_t *)(de->Value + i)) );
            }
        }

    } else if (type == SLONG) {

        if (value_count <= 1) {     // Value presents
            value = de->Value_Offset;
            de->Value_Offset = 0;
            de->Value = calloc(value_count, 4);
            memcpy(de->Value, &value, 4*value_count);
        } else {                    // Offset presents
            if (need_byte_swap) {
                offset = __builtin_bswap32(de->Value_Offset);
            } else {
                offset = de->Value_Offset;
            }
            de->Value_Offset = 0;
            de->Value = calloc(value_count, 4);
            memcpy(de->Value, ifh + offset, 4*value_count);
        }

        if (need_byte_swap) {
            for (int i = 0; i < value_count; i++) {
                *((uint32_t *)(de->Value + i)) = __builtin_bswap32( *((uint32_t *)(de->Value + i)) );
            }
        }

    } else if (type == FLOAT) {

        if (value_count <= 1) {     // Value presents
            value = de->Value_Offset;
            de->Value_Offset = 0;
            de->Value = calloc(value_count, 4);
            memcpy(de->Value, &value, 4*value_count);
        } else {                    // Offset presents
            if (need_byte_swap) {
                offset = __builtin_bswap32(de->Value_Offset);
            } else {
                offset = de->Value_Offset;
            }
            de->Value_Offset = 0;
            de->Value = calloc(value_count, 4);
            memcpy(de->Value, ifh + offset, 4*value_count);
        }

        if (need_byte_swap) {
            for (int i = 0; i < value_count; i++) {
                *((float *)(de->Value + i)) = __builtin_bswap32( *((float *)(de->Value + i)) );
            }
        }

    } else if (type == RATIONAL) {

        /* Since a RATIONAL takes at least 8 bytes, we can only retrieve using the Offset. */
        if (need_byte_swap) {
            offset = __builtin_bswap32(de->Value_Offset);
        } else {
            offset = de->Value_Offset;
        }
        de->Value_Offset = 0;
        de->Value = calloc(2*value_count, 4); // a RATIONAL consists of two LONGs
        memcpy(de->Value, ifh + offset, 8*value_count);

        if (need_byte_swap) {
            for (int i = 0; i < 2*value_count; i++) {
                *((uint32_t *)(de->Value + i)) = __builtin_bswap32( *((uint32_t *)(de->Value + i)) );
            }
        }

    } else if (type == SRATIONAL) {

        /* Since SRATIONAL takes at least 8 bytes, we can only retrieve using the Offset. */
        if (need_byte_swap) {
            offset = __builtin_bswap32(de->Value_Offset);
        } else {
            offset = de->Value_Offset;
        }
        de->Value_Offset = 0;
        de->Value = calloc(2*value_count, 4); // a SRATIONAL consists of two SLONGs
        memcpy(de->Value, ifh + offset, 8*value_count);

        if (need_byte_swap) {
            for (int i = 0; i < 2*value_count; i++) {
                *((int32_t *)(de->Value + i)) = __builtin_bswap32( *((int32_t *)(de->Value + i)) );
            }
        }

    } else if (type == DOUBLE) {
        
        /* Since DOUBLE takes at least 8 bytes, we can only retrieve using the Offset. */
        if (need_byte_swap) {
            offset = __builtin_bswap32(de->Value_Offset);
        } else {
            offset = de->Value_Offset;
        }
        de->Value_Offset = 0;
        de->Value = calloc(value_count, 8);
        memcpy(de->Value, ifh + offset, value_count);

        if (need_byte_swap) {
            for (int i = 0; i < value_count; i++) {
                *((double *)(de->Value + i)) = __builtin_bswap64( *((double *)(de->Value + i)) );
            }
        }

    } else if (type == UNDEFINED) {
        /**
         * TODO:
         * So far only Tags MakerNote, FlashpixVersion and SceneType are of type UNDEFINED so this is not urgent.
         */
    }

    if (tag == 0x8769) {
        struct Image_File_Directory *exif_ifd = calloc(1, sizeof(struct Image_File_Directory));
        uint8_t                     *from     = ifh + *((uint32_t *)(de->Value));

        /* Construct Exif IFD and print information */
        exif_construct_ifd(exif_ifd, &from, ifh);
        image_file_directory_print_info(exif_ifd);
        
    } else if (tag == 0x8825) {
        struct Image_File_Directory *gps_ifd = calloc(1, sizeof(struct Image_File_Directory));
        uint8_t                     *from     = ifh + *((uint32_t *)(de->Value));

        /* Construct Exif IFD and print information */
        exif_construct_ifd(gps_ifd, &from, ifh);
        image_file_directory_print_info(gps_ifd);
    }
}

void image_file_directory_print_info(struct Image_File_Directory *ifd) {
    printf("┌─────────────────────────────┬───────────┬───────┬───────────────────────────────────────────────────┐\n");
    printf("│             Tag             │   Type    │ Count │                       Value                       │\n");
    printf("├─────────────────────────────┼───────────┼───────┼───────────────────────────────────────────────────┤\n");
    for (int i = 0; i < ifd->DE_Count; i++) {
        directory_entry_print_info(ifd->DE + i);

        if (i == ifd->DE_Count - 1) {
            printf("└─────────────────────────────┴───────────┴───────┴───────────────────────────────────────────────────┘\n");
        } else {
            printf("├─────────────────────────────┼───────────┼───────┼───────────────────────────────────────────────────┤\n");
        }
    }
}

void image_file_header_byte_swap(struct Image_File_Header *ifh) {
    ifh->Magic_Number = __builtin_bswap16(ifh->Magic_Number);
    ifh->IFD_Offset   = __builtin_bswap32(ifh->IFD_Offset);
}

uint8_t* exif_construct_de(struct Directory_Entry *to, uint8_t *from, uint16_t de_count, uint8_t *ifh) {
    uint8_t *ptr = from; // pointer to the current byte

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

void exif_construct_ifd(struct Image_File_Directory *ifd, uint8_t **ptr, uint8_t *ifh) {
    uint16_t               de_count   = 0;    // count of DE
    uint32_t               ifd_offset = 0;    // offset of IFD
    struct Directory_Entry *de        = NULL; // pointer to the current DE

    /* Obtain the DE Count field of the IFD */
    memcpy(&de_count, *ptr, 2);
    if (need_byte_swap) {
        de_count = __builtin_bswap16(de_count);
    }
    ifd->DE_Count = de_count;

    /* Skip the DE Count field, now pointing at the first Directory Entry (DE) */
    *ptr += 2; 

    /* Construct the DEs */
    de     = calloc(de_count, sizeof(struct Directory_Entry));
    *ptr   = exif_construct_de(de, *ptr, de_count, ifh);
    ifd->DE = de;

    /* Obtain the IFD Offset field of the IFD */
    memcpy(&ifd_offset, *ptr, 4);
    if (need_byte_swap) {
        ifd_offset = __builtin_bswap32(ifd_offset);
    }
    ifd->IFD_Offset = ifd_offset;

    /**
     * Set the offset to point to the next IFD if exists, otherwise it means that there is no more IFD and hence the 
     * Marker Segment ends.
     */
    if (ifd_offset > 0) {
        *ptr = ifh + ifd_offset;
    } // if this is the last IFD, offset value would not be useful so nothing is done here
}

void exif_construct_segment(struct Exif_Segment *seg, uint8_t **ptr, uint16_t seg_len) {
    uint8_t                     *ifh      = NULL; // pointer to the first byte of the IFH
    struct Image_File_Directory *ifd      = NULL; // pointer to the first IFD
    struct Image_File_Directory *curr_ifd = NULL; // pointer to the current IFD
    struct Image_File_Directory *next_ifd = NULL; // pointer to the next IFD

    /* Obtain the Identifier field of the Exif Segment */
    memcpy(seg->Identifier, *ptr, 6);

    /* Skip the Identifier field, now pointing at the Byte Order field */
    *ptr += 6;

    /* Obtain the Image File Header */
    ifh = *ptr;
    memcpy(&(seg->IFH), *ptr, 8);
    if (seg->IFH.Byte_Order == BYTE_ORDER_BIG_ENDIAN) {
        need_byte_swap = true;
        image_file_header_byte_swap(&(seg->IFH));
    }

    /* Skip the IFH and IFD offset if exists, now pointing at the 1st IFD */
    *ptr += seg->IFH.IFD_Offset;

    /* Construct the 1st IFD and print the information */
    ifd = calloc(1, sizeof(struct Image_File_Directory));
    exif_construct_ifd(ifd, ptr, ifh); // pointing at the next IFD if exists
    image_file_directory_print_info(ifd);
    
    /* Construct the remaining IFDs if exist and print the information */
    curr_ifd = ifd; 
    while (curr_ifd->IFD_Offset != 0) { // The IFD Offset field of the last IFD is 0
        next_ifd = curr_ifd->Next_IFD;
        next_ifd = calloc(1, sizeof(struct Image_File_Directory));
        // *ptr = exif_construct_ifd(next_ifd, *ptr, ifh);
        exif_construct_ifd(next_ifd, ptr, ifh);
        image_file_directory_print_info(next_ifd);
        curr_ifd = next_ifd;
    }

    /* Point to the next Marker Segment */
    *ptr = *ptr + seg_len;
}
