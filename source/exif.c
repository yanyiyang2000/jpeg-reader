#include <stdbool.h>    // true, false
#include <stdint.h>     // uintXX_t, intXX_t
#include <stdio.h>      // printf
#include <stdlib.h>     // free
#include <string.h>     // memcpy
#include <strings.h>    // bzero
#include <inttypes.h>   // PRIX

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
    char *tag_name = NULL; // field tag name

    /* Obtain the field tag name */
    for (uint8_t i = 0; i < (sizeof(tags)/sizeof(struct Tag)); i++) {
        if (tags[i].Number == de->Tag) {
            tag_name = tags[i].Name;
            break;
        }
    }

    /* In case of unknown field tag number, print the tag number in hex */
    if (tag_name == NULL) {
        printf("│ 0x%-25"PRIX16" │ %-9s │ %-5s │ %-49s │\n", de->Tag, "", "", "");
    }

    /* Print the field tag name, field type, field value count and field values */
    if (de->Type == BYTE) {

        uint8_t *ptr = de->Value;
        printf("│ %-27s │ %-9s │ %-5"PRIu32" │ %-49"PRIu8" │\n", tag_name, "BYTE", de->Value_Count, *ptr);
        for (uint8_t i = 1; i < de->Value_Count; i++) {
            printf("│ %-27s | %-9s │ %-5s │ %-49"PRIu8" │\n", "", "", "", *(ptr + i));
        }

    } else if (de->Type == SBYTE) {

        int8_t *ptr = de->Value;
        printf("│ %-27s │ %-9s │ %-5"PRIu32" │ %-49"PRId8" │\n", tag_name, "SBYTE", de->Value_Count, *ptr);
        for (uint8_t i = 1; i < de->Value_Count; i++) {
            printf("│ %-27s | %-9s │ %-5s │ %-49"PRId8" │\n", "", "", "", *(ptr + i));
        }

    } else if (de->Type == ASCII) {

        printf("│ %-27s │ %-9s │ %-5"PRIu32" │ %-49s │\n", tag_name, "ASCII", de->Value_Count, (char *)(de->Value));

    } else if (de->Type == SHORT) {

        uint16_t *ptr = de->Value;
        printf("│ %-27s │ %-9s │ %-5"PRIu32" │ %-49"PRIu16" │\n", tag_name, "SHORT", de->Value_Count, *ptr);
        for (uint8_t i = 1; i < de->Value_Count; i++) {
            printf("│ %-27s | %-9s │ %-5s │ %-49"PRIu16" │\n", "", "", "", *(ptr + i));
        }

    } else if (de->Type == SSHORT) {

        int16_t *ptr = de->Value;
        printf("│ %-27s │ %-9s │ %-5"PRIu32" │ %-49"PRId16" │\n", tag_name, "SSHORT", de->Value_Count, *ptr);
        for (uint8_t i = 1; i < de->Value_Count; i++) {
            printf("│ %-27s | %-9s │ %-5s │ %-49"PRId16" │\n", "", "", "", *(ptr + i));
        }

    } else if (de->Type == LONG) {

        uint32_t *ptr = de->Value;
        printf("│ %-27s │ %-9s │ %-5"PRIu32" │ %-49"PRIu32" │\n", tag_name, "LONG", de->Value_Count, *ptr);
        for (uint8_t i = 1; i < de->Value_Count; i++) {
            printf("│ %-27s | %-9s │ %-5s │ %-49"PRIu32" │\n", "", "", "", *(ptr + i));
        }

    } else if (de->Type == SLONG) {

        int32_t *ptr = de->Value;
        printf("│ %-27s │ %-9s │ %-5"PRIu32" │ %-49"PRId32" │\n", tag_name, "SLONG", de->Value_Count, *ptr);
        for (uint8_t i = 1; i < de->Value_Count; i++) {
            printf("│ %-27s | %-9s │ %-5s │ %-49"PRId32" │\n", "", "", "", *(ptr + i));
        }

    } else if (de->Type == RATIONAL) {

        uint32_t *ptr = de->Value;
        /* Uncomment to print in fraction */
        // printf("│ %-27s │ %-9s │ %-5"PRIu32" │ %24"PRIu32"/%-24"PRIu32" │\n", tag_name, "RATIONAL", value_count, *ptr, *(ptr + 1));
        printf("│ %-27s │ %-9s │ %-5"PRIu32" │ %-49.2f │\n", tag_name, "RATIONAL", de->Value_Count, (double)(*ptr)/(*(ptr + 1)));
        for (uint8_t i = 1; i < de->Value_Count; i++) {
            // printf("│ %-27s | %-9s │ %-5s │ %24"PRIu32"/%-24"PRIu32" │\n", "", "", "", *(ptr + i), *(ptr + 2*i + 1));
            printf("│ %-27s | %-9s │ %-5s │ %-49.2f │\n", "", "", "", (double)(*(ptr + i))/(*(ptr + 2*i + 1)));
        }

    } else if (de->Type == SRATIONAL) {

        int32_t *ptr = de->Value;
        /* Uncomment to print in fraction */
        // printf("│ %-27s │ %-9s │ %-5"PRIu32" │ %24"PRId32"/%-24"PRId32" │\n", tag_name, "SRATIONAL", value_count, *ptr, *(ptr + 1));
        printf("│ %-27s │ %-9s │ %-5"PRIu32" │ %-49.2f │\n", tag_name, "SRATIONAL", de->Value_Count, (double)(*ptr)/(*(ptr + 1)));
        for (uint8_t i = 1; i < de->Value_Count; i++) {
            // printf("│ %-27s | %-9s │ %-5s │ %24"PRId32"/%-24"PRId32" │\n", "", "", "", *(ptr + i), *(ptr + 2*i + 1));
            printf("│ %-27s | %-9s │ %-5s │ %-49.2f │\n", "", "", "", (double)(*(ptr + i))/(*(ptr + 2*i + 1)));
        }

    } else if (de->Type == FLOAT) {

        float *ptr = de->Value;
        printf("│ %-27s │ %-9s │ %-5"PRIu32" │ %-49f │\n", tag_name, "FLOAT", de->Value_Count, *ptr);
        for (uint8_t i = 1; i < de->Value_Count; i++) {
            printf("│ %-27s | %-9s │ %-5s │ %-49f │\n", "", "", "", *(ptr + i));
        }
        
    } else if (de->Type == DOUBLE) {

        double *ptr = de->Value;
        printf("│ %-27s │ %-9s │ %-5"PRIu32" │ %-49f │\n", tag_name, "DOUBLE", de->Value_Count, *ptr);
        for (uint8_t i = 1; i < de->Value_Count; i++) {
            printf("│ %-27s | %-9s │ %-5s │ %-49f │\n", "", "", "", *(ptr + i));
        }

    } else if (de->Type == UNDEFINED) {
        printf("│ %-27s │ %-9s │ %-5"PRIu32" │ %-49s │\n", tag_name, "UNDEFINED", de->Value_Count, "");
    }
}

void directory_entry_parse_value(struct Exif_Segment *seg, struct Directory_Entry *de, uint8_t *ifh) {
    uint32_t offset = 0; // value offset
    uint32_t value  = 0; // actual value

    /**
     * Obtain the Value or Offset of the Value by evaluating the Value Offset field.
     * 
     * If sizeof(Type) * Value_Count <= 4, replace the Value_Offset field with the actual data, 
     * otherwise allocate memory for data type indicated by the Type field and copy the Value using Value_Offset.
     */
    if ( (de->Type == BYTE) | (de->Type == ASCII) | (de->Type == UNDEFINED) ) {

        if (de->Value_Count <= 4) { // Value presents
            value = de->Value_Offset;
            de->Value_Offset = 0;
            de->Value = calloc(de->Value_Count, 1);
            memcpy(de->Value, &value, de->Value_Count);
        } else {                    // Offset presents
            if (need_byte_swap) {
                offset = __builtin_bswap32(de->Value_Offset);
            } else {
                offset = de->Value_Offset;
            }
            de->Value_Offset = 0;
            de->Value = calloc(de->Value_Count, 1);
            memcpy(de->Value, ifh + offset, de->Value_Count);
        }

    } else if (de->Type == SBYTE) {

        if (de->Value_Count <= 4) { // Value presents
            value = de->Value_Offset;
            de->Value_Offset = 0;
            de->Value = calloc(de->Value_Count, 1);
            memcpy(de->Value, &value, de->Value_Count);
        } else {                    // Offset presents
            if (need_byte_swap) {
                offset = __builtin_bswap32(de->Value_Offset);
            } else {
                offset = de->Value_Offset;
            }
            de->Value_Offset = 0;
            de->Value = calloc(de->Value_Count, 1);
            memcpy(de->Value, ifh + offset, de->Value_Count);
        }

    } else if (de->Type == SHORT) {

        if (de->Value_Count <= 2) { // Value presents
            value = de->Value_Offset;
            de->Value_Offset = 0;
            de->Value = calloc(de->Value_Count, 2);
            memcpy(de->Value, &value, 2*de->Value_Count);
        } else {                    // Offset presents
            if (need_byte_swap) {
                offset = __builtin_bswap32(de->Value_Offset);
            } else {
                offset = de->Value_Offset;
            }
            de->Value_Offset = 0;
            de->Value = calloc(de->Value_Count, 2);
            memcpy(de->Value, ifh + offset, 2*de->Value_Count);
        }

        if (need_byte_swap) {
            for (int i = 0; i < de->Value_Count; i++) {
                *((uint16_t *)(de->Value + i)) = __builtin_bswap16( *((uint16_t *)(de->Value + i)) );
            }
        }

    } else if (de->Type == SSHORT) {

        if (de->Value_Count <= 2) { // Value presents
            value = de->Value_Offset;
            de->Value_Offset = 0;
            de->Value = calloc(de->Value_Count, 2);
            memcpy(de->Value, &value, 2*de->Value_Count);
        } else {                    // Offset presents
            if (need_byte_swap) {
                offset = __builtin_bswap32(de->Value_Offset);
            } else {
                offset = de->Value_Offset;
            }
            de->Value_Offset = 0;
            de->Value = calloc(de->Value_Count, 2);
            memcpy(de->Value, ifh + offset, 2*de->Value_Count);
        }

        if (need_byte_swap) {
            for (int i = 0; i < de->Value_Count; i++) {
                *((int16_t *)(de->Value + i)) = __builtin_bswap16( *((int16_t *)(de->Value + i)) );
            }
        }

    } else if (de->Type == LONG) {

        if (de->Value_Count <= 1) { // Value presents
            value = de->Value_Offset;
            de->Value_Offset = 0;
            de->Value = calloc(de->Value_Count, 4);
            memcpy(de->Value, &value, 4*de->Value_Count);
        } else {                    // Offset presents
            if (need_byte_swap) {
                offset = __builtin_bswap32(de->Value_Offset);
            } else {
                offset = de->Value_Offset;
            }
            de->Value_Offset = 0;
            de->Value = calloc(de->Value_Count, 4);
            memcpy(de->Value, ifh + offset, 4*de->Value_Count);
        }

        if (need_byte_swap) {
            for (int i = 0; i < de->Value_Count; i++) {
                *((uint32_t *)(de->Value + i)) = __builtin_bswap32( *((uint32_t *)(de->Value + i)) );
            }
        }

    } else if (de->Type == SLONG) {

        if (de->Value_Count <= 1) { // Value presents
            value = de->Value_Offset;
            de->Value_Offset = 0;
            de->Value = calloc(de->Value_Count, 4);
            memcpy(de->Value, &value, 4*de->Value_Count);
        } else {                    // Offset presents
            if (need_byte_swap) {
                offset = __builtin_bswap32(de->Value_Offset);
            } else {
                offset = de->Value_Offset;
            }
            de->Value_Offset = 0;
            de->Value = calloc(de->Value_Count, 4);
            memcpy(de->Value, ifh + offset, 4*de->Value_Count);
        }

        if (need_byte_swap) {
            for (int i = 0; i < de->Value_Count; i++) {
                *((uint32_t *)(de->Value + i)) = __builtin_bswap32( *((uint32_t *)(de->Value + i)) );
            }
        }

    } else if (de->Type == FLOAT) {

        if (de->Value_Count <= 1) { // Value presents
            value = de->Value_Offset;
            de->Value_Offset = 0;
            de->Value = calloc(de->Value_Count, 4);
            memcpy(de->Value, &value, 4*de->Value_Count);
        } else {                    // Offset presents
            if (need_byte_swap) {
                offset = __builtin_bswap32(de->Value_Offset);
            } else {
                offset = de->Value_Offset;
            }
            de->Value_Offset = 0;
            de->Value = calloc(de->Value_Count, 4);
            memcpy(de->Value, ifh + offset, 4*de->Value_Count);
        }

        if (need_byte_swap) {
            for (int i = 0; i < de->Value_Count; i++) {
                *((float *)(de->Value + i)) = __builtin_bswap32( *((float *)(de->Value + i)) );
            }
        }

    } else if (de->Type == RATIONAL) {

        /* Since a RATIONAL takes at least 8 bytes, we can only retrieve using the Offset. */
        if (need_byte_swap) {
            offset = __builtin_bswap32(de->Value_Offset);
        } else {
            offset = de->Value_Offset;
        }
        de->Value_Offset = 0;
        de->Value = calloc(2*de->Value_Count, 4); // a RATIONAL consists of two LONGs
        memcpy(de->Value, ifh + offset, 8*de->Value_Count);

        if (need_byte_swap) {
            for (int i = 0; i < 2*de->Value_Count; i++) {
                *((uint32_t *)(de->Value + i)) = __builtin_bswap32( *((uint32_t *)(de->Value + i)) );
            }
        }

    } else if (de->Type == SRATIONAL) {

        /* Since SRATIONAL takes at least 8 bytes, we can only retrieve using the Offset. */
        if (need_byte_swap) {
            offset = __builtin_bswap32(de->Value_Offset);
        } else {
            offset = de->Value_Offset;
        }
        de->Value_Offset = 0;
        de->Value = calloc(2*de->Value_Count, 4); // a SRATIONAL consists of two SLONGs
        memcpy(de->Value, ifh + offset, 8*de->Value_Count);

        if (need_byte_swap) {
            for (int i = 0; i < 2*de->Value_Count; i++) {
                *((int32_t *)(de->Value + i)) = __builtin_bswap32( *((int32_t *)(de->Value + i)) );
            }
        }

    } else if (de->Type == DOUBLE) {
        
        /* Since DOUBLE takes at least 8 bytes, we can only retrieve using the Offset. */
        if (need_byte_swap) {
            offset = __builtin_bswap32(de->Value_Offset);
        } else {
            offset = de->Value_Offset;
        }
        de->Value_Offset = 0;
        de->Value = calloc(de->Value_Count, 8);
        memcpy(de->Value, ifh + offset, de->Value_Count);

        if (need_byte_swap) {
            for (int i = 0; i < de->Value_Count; i++) {
                *((double *)(de->Value + i)) = __builtin_bswap64( *((double *)(de->Value + i)) );
            }
        }

    } else if (de->Type == UNDEFINED) {
        /**
         * TODO: So far only Tags MakerNote, FlashpixVersion and SceneType are of type UNDEFINED so this is not urgent.
         */
    }

    if (de->Tag == 0x8769) {
        seg->Exif_IFD = calloc(1, sizeof(struct Image_File_Directory));
        uint8_t *ptr  = ifh + *((uint32_t *)(de->Value));

        /* Construct Exif IFD and print information */
        exif_construct_ifd(seg, seg->Exif_IFD, &ptr, ifh);
        image_file_directory_print_info(seg->Exif_IFD);
        
    } else if (de->Tag == 0x8825) {
        seg->GPS_IFD = calloc(1, sizeof(struct Image_File_Directory));
        uint8_t *ptr  = ifh + *((uint32_t *)(de->Value));

        /* Construct Exif IFD and print information */
        exif_construct_ifd(seg, seg->GPS_IFD, &ptr, ifh);
        image_file_directory_print_info(seg->GPS_IFD);
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

void exif_construct_de(struct Exif_Segment *seg, struct Image_File_Directory *ifd, uint8_t **ptr, uint8_t *ifh) {
    for (uint16_t i = 0; i < ifd->DE_Count; i++) {
        memcpy(ifd->DE + i, *ptr, 12);
        if (need_byte_swap) {
            directory_entry_byte_swap(ifd->DE + i);
        }
        directory_entry_parse_value(seg, ifd->DE + i, ifh);
        *ptr += 12;
    }
}

void exif_construct_ifd(struct Exif_Segment *seg, struct Image_File_Directory *ifd, uint8_t **ptr, uint8_t *ifh) {
    /* Obtain the DE Count field of the IFD */
    memcpy(&(ifd->DE_Count), *ptr, 2);
    if (need_byte_swap) {
        ifd->DE_Count = __builtin_bswap16(ifd->DE_Count);
    }

    /* Skip the DE Count field, now pointing at the first DE */
    *ptr += 2; 

    /* Construct the DEs and skip them */
    ifd->DE = calloc(ifd->DE_Count, sizeof(struct Directory_Entry));
    exif_construct_de(seg, ifd, ptr, ifh);

    /* Obtain the IFD Offset field of the IFD */
    memcpy(&(ifd->IFD_Offset), *ptr, 4);
    if (need_byte_swap) {
        ifd->IFD_Offset = __builtin_bswap32(ifd->IFD_Offset);
    }

    /**
     * Set the offset to point to the next IFD if exists, otherwise it means that there is no more IFD and hence the 
     * Marker Segment ends.
     */
    if (ifd->IFD_Offset > 0) {
        *ptr = ifh + ifd->IFD_Offset;
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

    /* Skip the IFH and IFD offset if exists, now pointing at the first IFD */
    *ptr += seg->IFH.IFD_Offset;

    /* Construct the first IFD, print the information and skip it */
    ifd = calloc(1, sizeof(struct Image_File_Directory));
    exif_construct_ifd(seg, ifd, ptr, ifh);
    image_file_directory_print_info(ifd);
    
    /* Construct the remaining IFDs if exist, print the information and skip them */
    curr_ifd = ifd; 
    while (curr_ifd->IFD_Offset != 0) { // The IFD Offset field of the last IFD is 0
        next_ifd = curr_ifd->Next_IFD;
        next_ifd = calloc(1, sizeof(struct Image_File_Directory));
        exif_construct_ifd(seg, next_ifd, ptr, ifh);
        image_file_directory_print_info(next_ifd);
        curr_ifd = next_ifd;
    }

    /* Skip the Exif Segment, now pointing at the next Marker Segment */
    *ptr = *ptr + seg_len;
}

void exif_free_segment(struct Exif_Segment *seg) {
    struct Image_File_Directory *prev_ifd = NULL;
    struct Image_File_Directory *curr_ifd = seg->IFDs;

    /* Free the memory dynamically allocated to IFDs */
    while (1) {
        /* Free the memory dynamically allocated to each Exif_Segment->IFD->DE->Value */
        for (uint16_t i = 0; i < curr_ifd->DE_Count; i++) {
            free(((curr_ifd->DE) + i)->Value);
        }

        /* Free the memory dynamically allocated to Exif_Segment->IFD->DE */
        free(curr_ifd->DE);

        /* Free the memory dynamically allocated to each Exif_Segment->IFD */
        prev_ifd = curr_ifd;
        curr_ifd = curr_ifd->Next_IFD;
        if (prev_ifd->IFD_Offset == 0) {
            free(prev_ifd);
            break;
        } else {
            free(prev_ifd);
        }
    }
    
    /* Free the memory dynamically allocated to Exif IFD */
    if (seg->Exif_IFD != NULL) {
        free(seg->Exif_IFD);
    }

    /* Free the memory dynamically allocated to GPS IFD */
    if (seg->GPS_IFD != NULL) {
        free(seg->GPS_IFD);
    }

    /* Free the memory dynamically allocated to Exif_Segment */
    free(seg);
}
